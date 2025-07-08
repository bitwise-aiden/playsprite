// Conversion tool for turning Aseprite files into a Playdate compatible format.
//
// Using the official Aseprite file spec as reference for reading ase/aseprite files.
// https://github.com/aseprite/aseprite/blob/main/docs/ase-file-specs.md

#include <stdio.h>
#include <stdlib.h>

#include "chunk.h"
#include "frame.h"
#include "header.h"


int
main(int argc, char *argv[]) {
    char *file = "example.ase";
    if (argc == 2) {
        file = argv[1];
    }

    FILE *fptr;
    fptr = fopen(file, "r");

    header_t header = parse_header(fptr);

    for (WORD f = 0; f < header.frames; ++f) {
        frame_t frame = parse_frame(fptr);

        for (DWORD c = 0; c < frame.chunk_count; ++c) {
            chunk_t chunk = parse_chunk(fptr);
        }
    }

    return 0;
}
