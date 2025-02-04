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

/*
 * Default functions pointed at from lexer funtion pointers.
 * */


/*
 * Method to get the next char from the filestream.
 * */
char compile_process_next_char(struct lex_process* lex_process) {
    struct compile_process* compiler = lex_process->compiler;
    compiler->pos.col += 1; // Moves to the next column
    char c = getc(compiler->cfile.fp); // Pulls character from filestream
    
    // If character is new line symbol increment line counter and set column counter to 0
    if(c == '\n') {
        compiler->pos.line += 1;
        compiler->pos.col = 0;
    }

    return c;
}


/*
 * Method to peek at the next char in the filestream
 * */
char compile_process_peek_char(struct lex_process* lex_process) {
    struct compile_process* compiler = lex_process-> compiler;
    char c = getc(compiler->cfile.fp); // Pulls char from filestream
    ungetc(c, compiler->cfile.fp); // Push character back to filestream
    return c;
}


/*
 * Method to push a a new char on to the filestream
 * */
void compile_process_push_char(struct lex_process* lex_process, char c) {
    struct compile_process* compiler = lex_process-> compiler;
    ungetc(c, compiler->cfile.fp); // Pushes provided char to the file stream
}
