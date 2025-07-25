#include <assert.h>
#include <stdlib.h>

#include "external/puff.h"
#include "rewriter.h"


#define LAYER_FLAG_VISIBLE 0b1
#define MAX_DEPTH 16
#define MAX_LAYERS 256


size_t
calculate_layer_visibility(const frame_t *p_frame, BYTE *out_layer_visibility) {
    size_t layer_count = 0;

    BYTE visibility_stack[MAX_DEPTH] = { 1 };
    size_t stack_top = 1;

    for (size_t c = 0; c < p_frame->chunk_count; ++c) {
        const chunk_t *p_chunk = &p_frame->chunks[c];

        if (p_chunk->type == CHUNK_LAYER) {
            const chunk_layer_t *p_layer = p_chunk->layer;

            int level = p_layer->child_level;
            stack_top = level + 1;

            BYTE parent_visibility = visibility_stack[level];
            BYTE visibility = parent_visibility && (p_layer->flags & LAYER_FLAG_VISIBLE);

            out_layer_visibility[layer_count++] = visibility;
            visibility_stack[stack_top] = visibility;
        }
    }

    return layer_count;
}


void
free_ir_file(ir_file_t *p_file) {
    for (size_t c = 0; c < p_file->cel_count; ++c) {
        free(p_file->cels[c].pixels);
    }

    free(p_file->cels);
    free(p_file->frames);
    free(p_file->tags);
}


ir_file_t *
rewrite_file(file_t *p_file) {
    ir_file_t *file = malloc(sizeof(ir_file_t));

    file->frame_count = p_file->frame_count;

    assert(file->frame_count > 0);
    file->frames = malloc(sizeof(ir_frame_t) * file->frame_count);

    BYTE layer_visibility[MAX_LAYERS];
    file->layer_count = calculate_layer_visibility(&p_file->frames[0], layer_visibility);

    assert(file->layer_count > 0);
    file->cels = malloc(sizeof(ir_cel_data_t) * file->frame_count * file->layer_count);
    file->cel_count = 0;

    ir_pixel_t *palette = NULL;
    size_t palette_count = 0;

    for (size_t f = 0; f < file->frame_count; ++f) {
        frame_t *p_frame = &p_file->frames[f];
        ir_frame_t *frame = &file->frames[f];

        frame->cel_count = file->layer_count;
        frame->cels = malloc(sizeof(ir_cel_instance_t) * frame->cel_count);

        for (size_t c = 0; c < p_frame->chunk_count; ++c) {
            chunk_t *p_chunk = &p_frame->chunks[c];

            switch (p_chunk->type) {
                case CHUNK_CEL: {
                    chunk_cel_t *p_cel = p_chunk->cel;

                    if (!layer_visibility[p_cel->layer]) {
                        continue;
                    }

                    ir_cel_instance_t *cel = &frame->cels[p_cel->layer];
                    cel->z_index = p_cel->z_index;

                    if (p_cel->type == CEL_LINKED) {
                        size_t frame_index = p_chunk->cel_linked->frame;
                        cel->data = file->frames[frame_index].cels[p_cel->layer].data;

                        assert(cel->data != NULL);
                        continue;
                    }

                    ir_cel_data_t *cel_data = frame->cels[p_cel->layer].data = &file->cels[file->cel_count++];
                    cel_data->x = p_cel->position_x;
                    cel_data->y = p_cel->position_y;
                    cel_data->width = p_chunk->cel_image->width;
                    cel_data->height = p_chunk->cel_image->height;

                    cel_data->pixel_count = cel_data->width * cel_data->height;
                    cel_data->pixels = malloc(sizeof(ir_pixel_t) * cel_data->pixel_count);

                    void *raw = p_chunk->cel_image->pixel_data;

                    if (p_cel->type == CEL_COMPRESSED_IMAGE) {
                        size_t source_len = (size_t)p_chunk->size - sizeof(chunk_cel_t) - sizeof(WORD) * 2;
                        void *source = raw + 2;

                        size_t dest_len = cel_data->width * cel_data->height * (p_file->header.color_depth / sizeof(BYTE));
                        raw = malloc(dest_len);

                        int output = puff(raw, &dest_len, source, &source_len);
                        assert(output == 0);
                    }

                    BYTE *data = (BYTE *)raw;

                    for (size_t p = 0; p < cel_data->pixel_count; ++p) {
                        switch(p_file->header.color_depth) {
                            case COLOR_DEPTH_RGBA: {
                                cel_data->pixels[p].r = *data++;
                                cel_data->pixels[p].g = *data++;
                                cel_data->pixels[p].b = *data++;
                                cel_data->pixels[p].a = *data++;
                            } break;
                            case COLOR_DEPTH_GRAYSCALE: {
                                cel_data->pixels[p].r = cel_data->pixels[p].g = cel_data->pixels[p].b = *data++;
                                cel_data->pixels[p].a = *data++;
                            } break;
                            case COLOR_DEPTH_INDEXED: {
                                BYTE index = *data++;
                                cel_data->pixels[p] = palette[index];
                                cel_data->pixels[p].a *= (index != p_file->header.palette_entry);
                            } break;

                            default: {
                                assert(0);
                            } break;
                        }
                    }

                    if (p_cel->type == CEL_COMPRESSED_IMAGE) {
                        free(raw);
                    }
                } break;

                case CHUNK_PALETTE_OLD_1:
                case CHUNK_PALETTE_OLD_2: {
                    assert(palette == NULL);

                    BYTE *raw = p_chunk->data;

                    WORD packet_count = *(WORD *)raw;
                    raw += sizeof(WORD);

                    palette = malloc(sizeof(ir_pixel_t) * 256);

                    for (size_t p = 0; p < packet_count; ++p) {
                        BYTE skip = *raw++;
                        BYTE color_count = *raw++;
                        BYTE max_index = color_count - 1;

                        assert(skip + max_index <= 255);

                        for (size_t c = 0; c <= max_index; ++c) {
                            size_t index = skip + c;

                            palette[index].r = *raw++;
                            palette[index].g = *raw++;
                            palette[index].b = *raw++;
                            palette[index].a = 255;

                            ++palette_count;
                        }
                    }
                } break;

                case CHUNK_PALETTE: {
                    assert(palette == NULL);

                    const chunk_palette_t *p_palette = p_chunk->palette;

                    palette_count = p_palette->size;
                    palette = malloc(sizeof(ir_pixel_t) * palette_count);

                    BYTE *color_ptr = (BYTE *)p_palette + sizeof(chunk_palette_t);

                    for (size_t p = p_palette->index_first; p <= p_palette->index_last; ++p) {
                        WORD has_name = *(WORD *)color_ptr;
                        color_ptr += sizeof(WORD);

                        palette[p].r = *color_ptr++;
                        palette[p].g = *color_ptr++;
                        palette[p].b = *color_ptr++;
                        palette[p].a = *color_ptr++;

                        while (has_name && *color_ptr++ != '\0');
                    }
                } break;

                default: {} break;
            }
        }
    }

    if (palette != NULL) {
        free(palette);
    }

    return file;
}
