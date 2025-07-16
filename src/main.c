// Conversion tool for turning Aseprite files into a Playdate compatible format.
//
// Using the official Aseprite file spec as reference for reading ase/aseprite files.
// https://github.com/aseprite/aseprite/blob/main/docs/ase-file-specs.md

#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "rewriter.h"

int
main(int argc, char *argv[]) {
    char *file_path = "example.ase";
    if (argc == 2) {
        file_path = argv[1];
    }

    FILE *pfile;
    pfile = fopen(file_path, "r");

    file_t *file = parse_file(pfile);
    fclose(pfile);

    ir_file_t *ir_file = rewrite_file(file);
    free_parse_file(file);

    free_ir_file(ir_file);

    return 0;
}
