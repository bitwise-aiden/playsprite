#ifndef HEADER_H
#define HEADER_H


#include "types.h"


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


header_t parse_header(FILE *fptr);


#endif // HEADER_H
