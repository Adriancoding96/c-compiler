#include <stdlib.h>
#include <stdio.h>
#include "compiler.h"

// Creates a compile_process
struct compile_process *compile_process_create(const char *filename,
                                               const char *filename_out,
                                               int flags) {
    // Opens file
    FILE *file = fopen(filename, "r");

    // If file is null return null
    if (!file) {
        return NULL;
    }

    // Creates a output file
    FILE *out_file = NULL;
    if (!filename_out) { //If filename_out is not provided we dont write the compiled file
        if (!out_file) {
            return NULL;
        }
    }

    // Creates the compile_process
    struct compile_process* process = calloc(1, sizeof(struct compile_process));
    process->flags = flags; // Sets flags of the compile_process
    process->cfile.fp = file; // Sets input file
    process->ofile = out_file; // Sets output file

    return process;
}
