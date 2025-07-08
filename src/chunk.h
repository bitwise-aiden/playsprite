#ifndef CHUNK_H
#define CHUNK_H


#include "types.h"


typedef struct __attribute__((packed)) {
    WORD flags;
    WORD type;
    WORD child_level;
    WORD ignored_a;
    WORD ignored_b;
    WORD blend_mode;
    BYTE opacity;
    BYTE future[3];
} chunk_layer_t;


typedef struct __attribute__((packed)) {
    WORD  layer;
    SHORT position_x;
    SHORT position_y;
    BYTE  opacity;
    WORD  type;
    SHORT z_index;
    BYTE  future[5];
} chunk_cell_t;


typedef struct __attribute__((packed)) {
    DWORD size;
    WORD  type;
} chunk_header_t;


typedef struct __attribute__((packed)) {
} chunk_unparsed_t;


typedef union {
    chunk_layer_t    layer;
    chunk_cell_t     cell;
    chunk_unparsed_t unparsed;
} chunk_body_t;


typedef struct __attribute__((packed)) {
    chunk_header_t header;
    chunk_body_t   body;
} chunk_t;


chunk_t parse_chunk(FILE *fptr);

chunk_body_t   parse_chunk_body(FILE *fptr, chunk_header_t *header);
chunk_cell_t   parse_chunk_cell(FILE *fptr, chunk_header_t *header);
chunk_header_t parse_chunk_header(FILE *fptr);
chunk_layer_t  parse_chunk_layer(FILE *fptr, chunk_header_t *header);


#endif // CHUNK_H
