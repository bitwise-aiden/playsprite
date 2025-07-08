#include <stdio.h>

#include "chunk.h"


chunk_t
parse_chunk(FILE *fptr) {
    chunk_header_t header = parse_chunk_header(fptr);

    return (chunk_t) {
        .header = header,
        .body = parse_chunk_body(fptr, &header),
    };
}


chunk_body_t
parse_chunk_body(FILE* fptr, chunk_header_t *header) {
    switch(header->type) {
        case 0x2004: return (chunk_body_t){ .layer = parse_chunk_layer(fptr, header) };
        case 0x2005: return (chunk_body_t){ .cell = parse_chunk_cell(fptr, header) };
        default: {
            fseek(fptr, header->size, SEEK_CUR);
            return (chunk_body_t){ .unparsed = (chunk_unparsed_t){} };
        }
    }
}


chunk_cell_t
parse_chunk_cell(FILE *fptr, chunk_header_t *header) {
    chunk_cell_t chunk;
    fread(&chunk, sizeof(chunk_cell_t), 1, fptr);
    fseek(fptr, header->size - sizeof(chunk_cell_t), SEEK_CUR);

    printf(
        "cell - layer: %d, x: %d, y: %d, type: %d\n",
        chunk.layer,
        chunk.position_x,
        chunk.position_y,
        chunk.type
    );

    return chunk;
}


chunk_header_t
parse_chunk_header(FILE* fptr) {
    chunk_header_t header;
    fread(&header, sizeof(chunk_header_t), 1, fptr);

    header.size -= sizeof(chunk_header_t);

    printf(
        "chunk - type %#06x, size: %d\n",
        header.type,
        header.size
    );

    return header;
}


chunk_layer_t
parse_chunk_layer(FILE *fptr, chunk_header_t *header) {
    chunk_layer_t chunk;
    fread(&chunk, sizeof(chunk_layer_t), 1, fptr);
    fseek(fptr, header->size - sizeof(chunk_layer_t), SEEK_CUR);

    printf(
        "layer - type: %d, flags: %d\n",
        chunk.type,
        chunk.flags
    );

    return chunk;
}
