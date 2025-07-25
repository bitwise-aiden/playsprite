#ifndef PARSER_H
#define PARSER_H


#include <stdio.h>

#include "types.h"


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


typedef enum {
    CEL_RAW                = 0,
    CEL_LINKED             = 1,
    CEL_COMPRESSED_IMAGE   = 2,
    CEL_COMPRESSED_TILEMAP = 3,
} cel_type_t;


typedef struct __attribute__((packed)) {
    WORD  layer;
    SHORT position_x;
    SHORT position_y;
    BYTE  opacity;
    WORD  type;
    SHORT z_index;
    BYTE  future[5];
} chunk_cel_t;


typedef struct __attribute__((packed)) {
    chunk_cel_t cel;
    WORD frame;
} chunk_cel_linked_t;


typedef struct __attribute__((packed)) {
    chunk_cel_t cel;
    WORD width;
    WORD height;
    BYTE pixel_data[];
} chunk_cel_image_t;


typedef struct __attribute__((packed)) {
    WORD flags;
    WORD type;
    WORD child_level;
    WORD ignored_a;
    WORD ignored_b;
    WORD blend_mode;
    BYTE opacity;
    BYTE future[3];
    STRING name;
} chunk_layer_t;


typedef struct __attribute__((packed)) {
    DWORD size;
    DWORD index_first;
    DWORD index_last;
    BYTE future[8];
} chunk_palette_t;


typedef struct __attribute__((packed)) {
    DWORD size;
    WORD type;
} chunk_header_t;


typedef struct {
    chunk_type_t type;
    size_t size;
    union {
        void *data;
        chunk_cel_t *cel;
        chunk_cel_linked_t *cel_linked;
        chunk_cel_image_t *cel_image;
        chunk_layer_t *layer;
        chunk_palette_t *palette;
    };
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


void free_parse_file(file_t *p_file);
file_t *parse_file(FILE *p_file);


#endif // PARSER_H
