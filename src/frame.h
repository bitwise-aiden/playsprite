#ifndef FRAME_H
#define FRAME_H


#include "types.h"


typedef struct __attribute__((packed)) {
    DWORD bytes;
    WORD  magic_number;
    WORD  chunk_count_old;
    WORD  duration;
    BYTE  future[2];
    DWORD chunk_count;
} frame_t;


frame_t parse_frame(FILE *fptr);


#endif // FRAME_H
