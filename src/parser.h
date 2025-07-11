#ifndef PARSER_H
#define PARSER_H


# include "types.h"


typedef enum {
    CHUNK_PALETTE_OLD_1  = 0x0004,
    CHUNK_PALETTE_OLD_2  = 0x0011,
    CHUNK_LAYER          = 0x2004,
    CHUNK_CEL            = 0x2005,
    CHUNK_CEL_EXTRA      = 0x2006,
    CHUNK_COLOR_PROFILE  = 0x2007,
    CHUNK_EXTERNAL_FILES = 0x2008,
    CHUNK_MASK           = 0x2016,
    CHUNK_PATH           = 0x2017,
    CHUNK_TAGS           = 0x2018,
    CHUNK_PALETTE        = 0x2019,
    CHUNK_USER_DATA      = 0x2020,
    CHUNK_SLICE          = 0x2022,
    CHUNK_TILESET        = 0x2023,
} chunk_type_t;


typedef struct __attribute__((packed)) {
    DWORD size;
    WORD type;
} chunk_header_t;


typedef struct {
    chunk_type_t type;
    size_t size;
    void *data;
} chunk_t;


typedef struct __attribute__((packed)) {
    DWORD bytes;
    WORD  magic_number;
    WORD  chunk_count_old;
    WORD  duration;
    BYTE  future[2];
    DWORD chunk_count;
} frame_header_t;


typedef struct {
    frame_header_t header;
    size_t chunk_count;
    chunk_t *chunks;
} frame_t;


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
} file_header_t;


typedef struct {
    file_header_t header;
    size_t frame_count;
    frame_t *frames;
} file_t;


void free_file(file_t *p_file);
file_t *parse_file(FILE *p_file);


#endif // PARSER_H
