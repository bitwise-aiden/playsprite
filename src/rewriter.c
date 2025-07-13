#include <assert.h>
#include <stdlib.h>

#include "rewriter.h"


void
free_ir_file(ir_file_t *p_file) {
    free(p_file->cels);
    free(p_file->frames);
    free(p_file->tags);
}

ir_file_t *
rewrite_file(file_t *p_file) {
    ir_file_t *file = malloc(sizeof(ir_file_t));

    assert(p_file->frame_count > 0);

    file->frame_count = p_file->frame_count;
    file->frames = malloc(sizeof(ir_frame_t) * file->frame_count);

    for (size_t f = 0; f < p_file->frame_count; ++f) {
        frame_t *p_frame  = &p_file->frames[f];
        // ir_frame_t *frame = &file->frames[f];

        for (size_t c = 0; c < p_frame->chunk_count; ++c) {
            chunk_t *p_chunk = &p_frame->chunks[c];

            if (p_chunk->type == CHUNK_LAYER) {
                chunk_layer_t *layer = p_chunk->layer;
                printf("Layer(%s) - visible: %d, type: %d, level: %d\n", &layer->name.characters, layer->flags & 1, layer->type, layer->child_level);
            }
        }
    }

    // Iterate over frames, creating the each frame ahead of time
    // Figure out layer visibility
    // Sort out cels
    //   - ignoring cels when layer isn't visible
    //   - referencing previous frames when linked cell
    //      - should probably assert that first isn't linked
    // Sort out tags
    //   - figure out if user data is actually relevant for this or not

    return file;
}
