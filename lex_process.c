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

/*
 * Function to free memory allocated to a lex_process
 * */
void lex_proess_free(struct lex_process* process) {
    vector_free(process->token_vec); // Frees token vector
    free(process); // Frees lex process
}

/*
 * Gets the private data of the lex process void pointer
 * */
void* lex_rocess_private(struct lex_process* process) {
    return process->private;
}

/*
 * Returns the token of vector belonging to a lec process
 * */
struct vector* lex_process_tokens(struct lex_process* process) {
    return process->token_vec;
}
