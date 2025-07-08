#include <stdio.h>

#include "header.h"

header_t
parse_header(FILE *fptr) {
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

    return header;
}
