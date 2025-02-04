#include <stdio.h>

// Universal header file for project
#ifndef COMPILER_H
#define COMPILER_H

// Enum for compile process result
enum {
    COMPILER_FILE_COMPILED_OK,
    COMPILER_FAILED_WITH_ERRORS
};


// Struct defining the compile process
struct compile_process {
    // Flags for how file should be compiled
    int flags; 

    struct compile_process_input_file {
        FILE* fp; // Pointer to file
        const char* abs_path; // Path to file
    } cfile;

    // Create output file
    FILE* ofile;
};

int compile_file(const char* filename, const char* out_filename, int flags);

struct compile_process *compile_process_create(const char *filename,
                                               const char *filename_out,
                                               int flags);
#endif
