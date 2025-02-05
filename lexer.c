#include "compiler.h"
#include <string.h>
#include "helpers/vector.h"
#include "helpers/buffer.h"

/*
 * Macro to write characters to buffer.
 * Example usage: LEX_GETC_IF(buffer, current_char, (50 > 90))
 * */
#define LEX_GETC_IF(buffer, c, exp) {   \
    for(c = peekc(); exp; c = peekc()){ \
        buffer_write(buffer, c);        \
        nextc();                        \
    }                                   \
}


static struct lex_process* lex_process;

static struct token tmp_token;

// Uses function pointer to call peek_char function
static char peekc() {
    return lex_process->function->peek_char(lex_process);
}

/*
 * Function to read next char from file.
 * */
static char nextc() {
    char c = lex_process->function->next_char(lex_process); // Gets next char from input steam.
    lex_process->pos.col += 1; // Increments column state
    if(c == '\n') { // If char is new line character increments line state, and sets col state to 1
        lex_process->pos.line += 1;
        lex_process->pos.col = 1;
    }

    return c;
}

// Uses function pointer to call push_char function
static void pushc(char c) {
    lex_process->function->push_char(lex_process, c);
}

static struct pos lex_file_position() {
    return lex_process->pos;
}

struct token* token_create(struct token* _token) {
    memcpy(&tmp_token, _token, sizeof(struct token));
    tmp_token.pos = lex_file_position();
    return &tmp_token;
}

const char* read_number_str() {
    const char* num = NULL;
    struct buffer* buffer = buffer_create();
    char c = peekc();
    LEX_GETC_IF(buffer, c, (c >= '0' && c <= '9')); // Adds ascii string to buffer if it consists of numeric values
    buffer_write(buffer, 0x00); // Writes string terminator to buffer
    return buffer_ptr(buffer); // Return pointer to buffer data
}


unsigned long long read_number() {
    const char* s = read_number_str();
    return atoll(s); // Converts string to long
}

struct token* token_name_number_for_value(unsigned long number) {
    return token_create(&(struct token){.type=TOKEN_TYPE_NUMBER,.llnum=number});
}

struct token* token_make_number() {
    return token_name_number_for_value(read_number());
}

/*
 * Function creates and return token
 * */
struct token* read_next_token() {
    struct token* token = NULL;
    char c = peekc();
    switch(c) {
        NUMERIC_CASE: { // See case difinition in compiler.h
            token = token_make_number();
            break;
        }
        case EOF: {
            break; // Reached end of file   
        }
        default: {
            compiler_error(lex_process->compiler, "Unexpected token\n");
        }
    
    }


    return NULL;
}

int lex(struct lex_process* process) {

    process->current_expression_count = 0; // Initiate expr count to 0
    process->parantheses_buffer = NULL; // Initiate paranthases buffer to null
    lex_process = process; // Pass values from process to static process
    process->pos.filename = process->compiler->cfile.abs_path; // Set file name of process

    struct token* token = read_next_token(); // Read first token in text stream
    while(token) { // Keep reading next token until no token is present
        vector_push(process->token_vec, token); // Push token on to vector
        printf("%i\n", token->cval);
        token = read_next_token();
    }

    return LEXICAL_ANALYSIS_ALL_OK;
}
