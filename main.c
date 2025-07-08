// Conversion tool for turning Aseprite files into a Playdate compatible format.
//
// Using the official Aseprite file spec as reference for reading ase/aseprite files.
// https://github.com/aseprite/aseprite/blob/main/docs/ase-file-specs.md

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


typedef char     BYTE;
typedef uint16_t WORD;
typedef short    SHORT;
typedef uint32_t DWORD;


typedef struct __attribute__((packed)) {
    DWORD size;
    WORD  magic_number;
    WORD  frames;
    WORD  width;
    WORD  height;
    WORD  color_depth;
    DWORD flags;
    WORD  speed;
    DWORD unknown_a;
    DWORD unknown_b;
    BYTE  palette_entry;
    BYTE  ignored[3];
    WORD  color_count;
    BYTE  pixel_width;
    BYTE  pixel_height;
    SHORT x_position;
    SHORT y_position;
    WORD  grid_width;
    WORD  grid_height;
    BYTE  future[84];
} header_t;


typedef struct __attribute__((packed)) {
    DWORD bytes;
    WORD  magic_number;
    WORD  chunk_count_old;
    WORD  duration;
    BYTE  future[2];
    DWORD chunk_count_new;
} frame_t;


typedef struct __attribute__((packed)) {
    DWORD size;
    WORD  type;
} chunk_header_t;


typedef struct __attribute__((packed)) {
    WORD flags;
    WORD type;
    WORD child_level;
    WORD ignored_a;
    WORD ignored_b;
    WORD blend_mode;
    BYTE opacity;
    BYTE future[3];
} layer_chunk_t;


typedef struct __attribute__((packed)) {
    WORD  layer;
    SHORT position_x;
    SHORT position_y;
    BYTE  opacity;
    WORD  type;
    SHORT z_index;
    BYTE  future[5];
} cell_chunk_t;


int main(int argc, char *argv[]) {
    char *file = "example.ase";
    if (argc == 2) {
        file = argv[1];
    }

    FILE *fptr;
    fptr = fopen(file, "r");

    header_t header;
    fread(&header, sizeof(header_t), 1, fptr);

    printf(
        "width:%d, height:%d, frames: %d, color_depth: %d, color_count: %d\n",
        header.width,
        header.height,
        header.frames,
        header.color_depth,
        header.color_count
    );

    for (WORD f = 0; f < header.frames; ++f) {
        frame_t frame;
        fread(&frame, sizeof(frame_t), 1, fptr);

        DWORD chunk_count = frame.chunk_count_old == 0xFFFF ? frame.chunk_count_new : frame.chunk_count_old;
        printf(
            "  frame: %d, size: %d, chunk: %d\n",
            f,
            frame.bytes,
            chunk_count
        );

        for (DWORD c = 0; c < chunk_count; ++c) {
            chunk_header_t chunk_header;
            fread(&chunk_header, sizeof(chunk_header_t), 1, fptr);
            chunk_header.size -= sizeof(chunk_header_t);

            printf(
                "    chunk: %d: type %#06x, size: %d\n",
                c,
                chunk_header.type,
                chunk_header.size
            );

            switch(chunk_header.type) {
                case 0x2004: {
                    layer_chunk_t chunk;
                    fread(&chunk, sizeof(layer_chunk_t), 1, fptr);
                    fseek(fptr, chunk_header.size - sizeof(layer_chunk_t), SEEK_CUR);

                    printf(
                        "      layer - type: %d, flags: %d\n",
                        chunk.type,
                        chunk.flags
                    );
                } break;
                case 0x2005: {
                    cell_chunk_t chunk;
                    fread(&chunk, sizeof(cell_chunk_t), 1, fptr);
                    fseek(fptr, chunk_header.size - sizeof(cell_chunk_t), SEEK_CUR);

                    printf(
                        "      cell - layer: %d, x: %d, y: %d, type: %d\n",
                        chunk.layer,
                        chunk.position_x,
                        chunk.position_y,
                        chunk.type
                    );
                } break;
                case 0x0004: {
                    WORD number;
                    fread(&number, sizeof(WORD), 1, fptr);

                    BYTE counts[2];
                    fread(&counts, sizeof(BYTE), 2, fptr);

                    printf(
                        "      palette - number: %d, counts: %d\n",
                        number,
                        counts[1]
                    );

                    fseek(fptr, chunk_header.size - sizeof(WORD) - sizeof(BYTE) * 2, SEEK_CUR);
                } break;
                default: {
                    fseek(fptr, chunk_header.size, SEEK_CUR);
                } break;
            }
        }
    }

    return 0;
}
