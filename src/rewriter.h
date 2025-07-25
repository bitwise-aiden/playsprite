#ifndef REWRITER_H
#define REWRITER_H


#include "parser.h"
#include "types.h"


typedef struct {
    BYTE r;
    BYTE g;
    BYTE b;
    BYTE a;
} ir_pixel_t;


typedef struct {
    SHORT x;
    SHORT y;
    WORD width;
    WORD height;

    ir_pixel_t *pixels;
    size_t pixel_count;
} ir_cel_data_t;


typedef struct {
    ir_cel_data_t* data;
    SHORT z_index;
} ir_cel_instance_t;


typedef struct {
    WORD duration;

    ir_cel_instance_t *cels;
    size_t cel_count;
} ir_frame_t;


typedef struct {
    WORD first_frame;
    WORD last_frame;
    char *name;
} ir_tag_t;


typedef struct {
    size_t layer_count;

    ir_cel_data_t *cels;
    size_t cel_count;

    ir_frame_t *frames;
    size_t frame_count;

    ir_tag_t *tags;
    size_t tag_count;
} ir_file_t;


void free_ir_file(ir_file_t *p_file);
ir_file_t *rewrite_file(file_t *p_file);


#endif // REWRITER_H
