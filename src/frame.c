#include <stdio.h>

#include "frame.h"


frame_t
parse_frame(FILE *fptr) {
    frame_t frame;
    fread(&frame, sizeof(frame_t), 1, fptr);

    if (frame.chunk_count == 0) {
        frame.chunk_count = frame.chunk_count_old;
    }

    printf(
        "frame - size: %d, chunk: %d\n",
        frame.bytes,
        frame.chunk_count
    );

    return frame;
}
