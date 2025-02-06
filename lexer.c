#include "compiler.h"
#include <assert.h>
#include <stdio.h>
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
static struct token* read_next_token();

// Uses function pointer to call peek_char function
static char peekc() {
    return lex_process->function->peek_char(lex_process);
}

/*
 * Function to read next char from file.
 * */
static char nextc() {
    char c = lex_process->function->next_char(lex_process); // Gets next char from input steam.
    //printf("DEBUG nextc, current char = %i\n", c);
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

// Returns position in file of current lex process
static struct pos lex_file_position() {
    return lex_process->pos;
}

/*
 * Takes a token and copys it on to temp_token
 * */
struct token* token_create(struct token* _token) {
    memcpy(&tmp_token, _token, sizeof(struct token));
    tmp_token.pos = lex_file_position();
    return &tmp_token;
}

/*
 * Returns the last token that was pushed to lexer
 * */
static struct token* lexer_last_token() {
    return vector_back_or_null(lex_process->token_vec);
}


static struct token* handle_whitespace() {
    struct token* last_token = lexer_last_token();
    if(last_token){
        last_token->whitespace = true;
    } 

    nextc();
    return read_next_token();
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


/*
 * Creates and returns a new token of long long
 * */
struct token* token_name_number_for_value(unsigned long number) {
    return token_create(&(struct token){.type=TOKEN_TYPE_NUMBER,.llnum=number});
}

struct token* token_make_number() {
    return token_name_number_for_value(read_number());
}

/*
 * Creates a string token, input paramaters start are the ""
 * encapsulating the string.
 * */
static struct token* token_make_string(char start, char end) {
    struct buffer* buf = buffer_create(); // Create new buffer
    assert(nextc() == start); // Make sure start character is " else assert will exit compile process
    char c = nextc(); // Initiate first char
    for(;c  != end && c != EOF; c = nextc()) { // Traverse file intil end char is fuound or EOF

        if(c == '\\') { // Handle escape symbol
            continue; // Temporary solution
        }
    }

    buffer_write(buf, c); // Add character to buffer
    buffer_write(buf, 0x00); // Add terminator to buffer

    // Create and returns string token with the value equal to the buffer pointer
    return token_create(&(struct token){.type=TOKEN_TYPE_STRING,.sval=buffer_ptr(buf)});
}


/*
 * Function creates and return token
 * */
struct token *read_next_token() {
    struct token* token = NULL;
    char c = peekc();
    switch(c) {
        NUMERIC_CASE: { // See case difinition in compiler.h
            //printf("DEBUG NUMERIC CASE char = %i\n", c);
            token = token_make_number();
            break;
        }
        case '"': {
            token = token_make_string('"', '"');
            break;
        }
        case ' ': { // Ignore spaces
            token = handle_whitespace();
            break;
        }
        case '\t': { // Ignore tabs
            token = handle_whitespace();
            break;
        }
        case '\n': { // Temporary i think nvim is inserting new line characters for some reason
            break;
        }
        case EOF: {
            break; // Reached end of file   
        }
        default: {
            //printf("DEBUG unexpected token = %i\n", c);
            compiler_error(lex_process->compiler, "Unexpected token\n");
        }
    
    }


    return token;
}

int lex(struct lex_process* process) {

    process->current_expression_count = 0; // Initiate expr count to 0
    process->parantheses_buffer = NULL; // Initiate paranthases buffer to null
    lex_process = process; // Pass values from process to static process
    process->pos.filename = process->compiler->cfile.abs_path; // Set file name of process

    struct token* token = read_next_token(); // Read first token in text stream
    while(token) { // Keep reading next token until no token is present
        vector_push(process->token_vec, token); // Push token on to vector
        token = read_next_token();
    }

    return LEXICAL_ANALYSIS_ALL_OK;
}
