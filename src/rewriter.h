#ifndef REWRITER_H
#define REWRITER_H


#include "parser.h"
#include "types.h"


typedef struct {

} ir_cel_t;


typedef struct {
    WORD duration;

    ir_cel_t *cels;
    size_t cel_count;
} ir_frame_t;


typedef struct {

} ir_tag_t;


typedef struct {
    ir_cel_t *cels;
    size_t cel_count;

    ir_frame_t *frames;
    size_t frame_count;

    ir_tag_t *tags;
    size_t tag_count;
} ir_file_t;


void free_ir_file(ir_file_t *p_file);
ir_file_t *rewrite_file(file_t *p_file);


#endif // REWRITER_H
