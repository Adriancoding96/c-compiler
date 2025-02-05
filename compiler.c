#include "compiler.h"

/*
 * Defines lexer functions.
 * */
struct lex_process_functions compiler_lex_function = {
    .next_char=compile_process_next_char,
    .peek_char=compile_process_peek_char,
    .push_char=compile_process_push_char,
};

int compile_file(const char* filename, const char* out_filename, int flags) {

    // Creates new compile process
    struct compile_process* process = compile_process_create(filename, out_filename, flags);
    
    // If process failes to be create return enum representing error
    if(!process) {
        return COMPILER_FAILED_WITH_ERRORS;
    }


    // Return enum representing successfull compile process
    return COMPILER_FILE_COMPILED_OK;
}
