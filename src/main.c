// Conversion tool for turning Aseprite files into a Playdate compatible format.
//
// Using the official Aseprite file spec as reference for reading ase/aseprite files.
// https://github.com/aseprite/aseprite/blob/main/docs/ase-file-specs.md

#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "rewriter.h"

int
main(int argc, char *argv[]) {
    char *file_path = "example.ase";
    if (argc == 2) {
        file_path = argv[1];
    }

    FILE *pfile;
    pfile = fopen(file_path, "r");

    file_t *file = parse_file(pfile);
    fclose(pfile);

    printf("file - frame_count: %zu\n", file->frame_count);

    // for (size_t f = 0; f < file->frame_count; ++f) {
    //     frame_t *frame = &file->frames[f];
    //     printf("  frame %zu - chunk count: %zu\n", f + 1, frame->chunk_count);

    //     for (size_t c = 0; c < frame->chunk_count; ++c) {
    //         chunk_t *chunk = &frame->chunks[c];
    //         printf("   chunk %zu - type: %#06x, size: %zu\n", c + 1, chunk->type, chunk->size);
    //     }
    // }

    ir_file_t *ir_file = rewrite_file(file);
    free_parse_file(file);

    // for (size_t f = 0; f < ir_file->frame_count; ++f) {
    //     ir_frame_t *p_frame = &ir_file->frames[f];

    //     for (size_t c = 0; c < p_frame->cel_count; ++c) {
    //         ir_cel_instance_t *p_cel = &p_frame->cels[c];
    //         ir_cel_data_t *p_data = p_cel->data;

    //         if (p_data == NULL) {
    //             continue;
    //         }

    //         printf("f: %zu, l: %zu, z: %d\n", f, c, p_cel->z_index);

    //         for (size_t p = 0; p < p_data->pixel_count; ++p) {
    //             printf("%03d", p_data->pixels[p].r);

    //             if (p % p_data->width == 0) {
    //                 printf("\n");
    //             }
    //         }

    //         printf("\n");
    //     }
    // }

    free_ir_file(ir_file);

    return 0;
}
