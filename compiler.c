#include "compiler.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Defines lexer functions.
 * */
struct lex_process_functions compiler_lex_functions = {
    .next_char=compile_process_next_char,
    .peek_char=compile_process_peek_char,
    .push_char=compile_process_push_char,
};

/*
 * Function to construct a compiler error printing it
 * and then terminates compiler.
 * */
void compiler_error(struct compile_process* compiler, const char* msg, ...) {
    va_list args; // Variable to hold list of arguments;
    va_start(args, msg); // Initiates args to acces list of arguments with msg as the last one
    vfprintf(stderr, msg, args); // function that lets you print a va list
    va_end(args); // Clean memory allocated for aditional arguments
    fprintf(stderr, " on line %i, col %i in file %s\n", compiler->pos.line, compiler->pos.col, compiler->pos.filename);

    exit(-1);
}

/*
 * Function to construct a compiler warning and printing it. 
 * */
void compiler_warning(struct compile_process* compiler, const char* msg, ...) {
    va_list args;
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fprintf(stderr, " on line %i, col %i in file %s\n", compiler->pos.line, compiler->pos.col, compiler->pos.filename);
}

int compile_file(const char* filename, const char* out_filename, int flags) {

    // Creates new compile process
    struct compile_process* process = compile_process_create(filename, out_filename, flags);
    
    // If process failes to be create return enum representing error
    if(!process) {
        return COMPILER_FAILED_WITH_ERRORS;
    }

    struct lex_process* lex_process = lex_process_create(process, &compiler_lex_functions, NULL);

    // Make sure creation of lex process was successfull
    if(!lex_process) {
        return COMPILER_FAILED_WITH_ERRORS;
    }

    // Make sure lexer successufully runs
    if(lex(lex_process) != LEXICAL_ANALYSIS_ALL_OK) {
        return COMPILER_FAILED_WITH_ERRORS;
    }

    // Return enum representing successfull compile process
    return COMPILER_FILE_COMPILED_OK;
}
