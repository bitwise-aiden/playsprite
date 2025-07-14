#include <assert.h>
#include <stdlib.h>

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
            const chunk_layer_t *layer = p_chunk->layer;
            printf("%zu: %s\n", layer_count, layer->name.characters);

            int level = layer->child_level;
            stack_top = level + 1;

            BYTE parent_visibility = visibility_stack[level];
            BYTE visibility = parent_visibility && (layer->flags & LAYER_FLAG_VISIBLE);

            out_layer_visibility[layer_count++] = visibility;
            visibility_stack[stack_top] = visibility;
        }
    }

    return layer_count;
}


void
free_ir_file(ir_file_t *p_file) {
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

    // Allocate the buffer for potential cels. Will never be larger than frame * layer count
    // so this is a safe upper bound.
    assert(file->layer_count > 0);
    file->cels = malloc(sizeof(ir_cel_t) * file->frame_count * file->layer_count);
    file->cel_count = 0;

    // Iterate over the frames and each of their chunks, pulling out the information required
    // to instantiate all of the cel data for each.
    for (size_t f = 0; f < file->frame_count; ++f) {
        frame_t *p_frame = &p_file->frames[f];
        ir_frame_t *frame = &file->frames[f];

        frame->cel_count = file->layer_count;
        frame->cels = malloc(sizeof(ir_cel_t *) * frame->cel_count);

        for (size_t c = 0; c < p_frame->chunk_count; ++c) {
            chunk_t *p_chunk = &p_frame->chunks[c];

            if (p_chunk->type == CHUNK_CEL) {
                chunk_cel_t *p_cel = p_chunk->cel;

                // If the layer for this cel is invisible, we can ignore it as it won't
                // make it into the final pass.
                if (!layer_visibility[p_cel->layer]) {
                    continue;
                }

                // If the cel is linked, look up the frame / layer pair that should already
                // be parsed, and reference that.
                if (p_cel->type == CEL_LINKED) {
                    size_t frame_index = p_chunk->cel_linked->frame;
                    frame->cels[p_cel->layer] = file->frames[frame_index].cels[p_cel->layer];

                    assert(frame->cels[p_cel->layer] != NULL);
                    continue;
                }

                ir_cel_t *cel = frame->cels[p_cel->layer] = &file->cels[file->cel_count++];
                cel->x = p_cel->position_x;
                cel->y = p_cel->position_y;
                cel->width = p_chunk->cel_image->width;
                cel->height = p_chunk->cel_image->height;

                printf("cel (f:%zu, l:%d): x: %d, y: %d, w: %d, h: %d\n", f, p_cel->layer, cel->x, cel->y, cel->width, cel->height);
            }
        }
    }

    return file;
}
