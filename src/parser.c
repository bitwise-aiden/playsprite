#include <stdio.h>
#include <stdlib.h>

#include "parser.h"


void
free_parse_file(file_t *p_file) {
    for (size_t f = 0; f < p_file->frame_count; ++f) {
        frame_t *frame = &p_file->frames[f];

        for (size_t c = 0; c < frame->chunk_count; ++c) {
            chunk_t *chunk = &frame->chunks[c];
            free(chunk->data);
        }

        free(frame->chunks);
    }

    free(p_file->frames);
    free(p_file);
}

file_t *
parse_file(FILE *p_file) {
    file_t *file = malloc(sizeof(file_t));

    fread(&file->header, sizeof(file_header_t), 1, p_file);

    file->frame_count = file->header.frames;
    file->frames = malloc(sizeof(frame_t) * file->frame_count);

    for (size_t f = 0; f < file->frame_count; ++f) {
        frame_t *frame = &file->frames[f];

        fread(&frame->header, sizeof(frame_header_t), 1, p_file);

        frame->chunk_count = frame->header.chunk_count == 0 ? frame->header.chunk_count_old : frame->header.chunk_count;
        frame->chunks = malloc(sizeof(chunk_t) * frame->chunk_count);

        for (size_t c = 0; c < frame->chunk_count; ++c) {
            chunk_t *chunk = &frame->chunks[c];

            chunk_header_t chunk_header;
            fread(&chunk_header, sizeof(chunk_header_t), 1, p_file);

            chunk->size = chunk_header.size - sizeof(chunk_header_t);
            chunk->type = chunk_header.type;
            chunk->data = malloc(chunk->size);

            fread(chunk->data, chunk->size, 1, p_file);
        }
    }

    return file;
}
