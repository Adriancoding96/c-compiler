#include <stdio.h>
#include <stdbool.h>

// Universal header file for project
#ifndef COMPILER_H
#define COMPILER_H

// Token struct for lexical analasys
struct token {
    int type;
    int flags;

    // Struct representing position of a token
    struct pos {
        int line; // Current line of token
        int col; // Current column of token
        const char* filename; // File name of token
    };

    // Value types token can obtain as a union to avoid allocating more memory then needed,
    // unions share the same space in memory.
    union { 
        char cval; // Char value
        const char* sval; // String value
        unsigned int inum; // Integer value
        unsigned long lnum; // Long value
        unsigned long long llnum; // Long long value
        void* any; // Void pointer, pointing to any type
    };

    // True if whitespace exists between this and next token, false if not.
    bool whitespace;

    // Pointer to start of buffer that exists between brackets
    const char* between_brackets;
};

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
