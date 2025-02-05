#include "compiler.h"
#include "helpers/vector.h"
#include <stdlib.h>


/*
 * Function that initiates a lexical analasys process
 * */
struct lex_process* lex_process_create(struct compile_process* compiler, struct lex_process_functions* functions, void* private) {
   
    // Initiate new lexical analasys process
    struct lex_process* process = calloc(1, sizeof(struct lex_process));
    process->function = functions; // Set function to dynamic function pointers
    process->token_vec =  vector_create(sizeof(struct token)); // Initiate token vector
    process->compiler = compiler; // Sets compilation process
    process->private = private; // Sets null pointer
    process->pos.line = 1; // Sets starting position on line
    process->pos.col = 1; // Sets starting position on column

    return process;
}
