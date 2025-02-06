#include "compiler.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
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
 * Function checks if operator can only be used once,
 * for example ?. is not a valid operator, but a combination like <= is valid.
 * */
static bool op_treated_as_one(char op) {
    return op == '(' 
        || op == '[' 
        || op == '.' 
        || op == ',' 
        || op == '*' 
        || op == '?';
}

/*
 * Function checks if the provided character is a single operator.
 * example if provided character is not one of theese it is a separate token
 * and does not need joining.
 * */
static bool is_single_op(char op) {
    return op == '+' 
        || op == '-' 
        || op == '/' 
        || op == '*' 
        || op == '=' 
        || op == '>' 
        || op == '<' 
        || op == '|' 
        || op == '&' 
        || op == '^' 
        || op == '%' 
        || op == '!' 
        || op == '(' 
        || op == '[' 
        || op == ',' 
        || op == '.' 
        || op == '~' 
        || op == '?';
}

/*
 * Function checks if string contains valid operator
 * */
bool op_valid(const char* op) {
    return S_EQ(op, "+")
        || S_EQ(op, "*")
        || S_EQ(op, "/")
        || S_EQ(op, "!")
        || S_EQ(op, "^")
        || S_EQ(op, "+=")
        || S_EQ(op, "-=")
        || S_EQ(op, "/=")
        || S_EQ(op, ">>")
        || S_EQ(op, "<<")
        || S_EQ(op, ">=")
        || S_EQ(op, "<=")
        || S_EQ(op, ">")
        || S_EQ(op, "<")
        || S_EQ(op, "||")
        || S_EQ(op, "&&")
        || S_EQ(op, "|")
        || S_EQ(op, "&")
        || S_EQ(op, "++")
        || S_EQ(op, "--")
        || S_EQ(op, "=")
        || S_EQ(op, "!=")
        || S_EQ(op, "==")
        || S_EQ(op, "->")
        || S_EQ(op, "(")
        || S_EQ(op, "[")
        || S_EQ(op, ",")
        || S_EQ(op, ".")
        || S_EQ(op, "...")
        || S_EQ(op, "~")
        || S_EQ(op, "?")
        || S_EQ(op, "%");
}

/*
 * Function returns trailing character of invalid operator to input stream.
 * example *+ is a invalid operator, + will be pushed back leaving only *.
 * */
void read_op_flush_back_keep_first(struct buffer* buf) {
    const char* data =buffer_ptr(buf); // Get string from buffer
    int n = buf->len; // Get length of buffer
    for(int i = n - 1; i >= 1; i--) { // Loop backwards through string
        if(data[i] == 0x00) { // If character is terminator continue
            continue;
        }
        pushc(data[i]); // Push char back to input stream

    }
}

/*
 * Function handles reading operators from text file, also handles
 * vverifying if single or joined operator.
 * */
const char* read_op() {
    bool single_operator = true;
    char op = nextc(); // Get next char from file
    struct buffer* buf = buffer_create(); // Create new buffer
    buffer_write(buf, op); // Write operator to buffer

    if(!op_treated_as_one(op)) { // Check if the next character is a operator that can be joined
        op = peekc(); // Check what next character in file is
        if(is_single_op(op)) { // Check if next character is also a operator
            buffer_write(buf, op);
            nextc(); // Pop of trailing operator
            single_operator = false;  // Set single operator flag to false
        }
    }

    buffer_write(buf ,0x00); // Write terminator to buffer
    char* ptr = buffer_ptr(buf);

    // Check if single operator
    if(!single_operator) {

        if(!op_valid(ptr)) { // Check if operator pair is valid
            read_op_flush_back_keep_first(buf); // Keep first character for operator tokenisation
                                             // pass trailing characters back to input stream.
            ptr[1] = 0x00; // Re insert null terminator as it has been discarded in read_op_back_keep_first function
        }
    }
    else if(!op_valid(ptr)) { // If operator is not valid throw compiler error
        compiler_error(lex_process->compiler, "The operator &i is not valid\n", ptr);
    }
    return ptr;

}

/*
 * Function finishes a expression by decremnting the lex process expression count.
 * Throws compiler error if expression is smaller than 0 meaning we have inserted an
 * expression closing symbol without a opening one.
 * */
static void lex_finish_expression() {
    lex_process->current_expression_count--;
    if(lex_process->current_expression_count < 0) { // If this is true we are trying to close a expression
                                                    // that was never opened, and wee need to throw a compiler error.
        compiler_error(lex_process->compiler, "You closed an expression that was never opened\n");

    }
}

/*
 * Function creates a new paranthases buffer for lex_process if a expression
 * such as ( is encounterd.
 * */
static void lex_new_expression() {
    lex_process->current_expression_count++; // Increment expression count of lexer
    if(lex_process->current_expression_count == 1) { // If expression count is equal to one we need to create a new
                                                     // paranthases buffer.
        lex_process->parantheses_buffer = buffer_create();
    }
}

/*
 * Function checks if the lex process is currently inside a expression.
 * */
bool lex_is_in_expression() {
    return lex_process->current_expression_count > 0;
}



/*
 * Function creates a operator token with the exceptions of #include statements
 * being returned as string tokens instead, or token being a start of a expression.
 * */
static struct token* token_make_operator_or_string() {
    char op = peekc();

    // Handle #include scenario which uses operator characters
    // making sure they get parsed as a string.
    if(op == '<') {
        struct token* last_token = lexer_last_token();
        if(token_is_keyword(last_token, "include")) {
            return token_make_string('<', '>');
        }
    }

    // Creates and return token of operator type with a string value
    struct token* token = token_create(&(struct token){.type = TOKEN_TYPE_OPERATOR, .sval = read_op()});
    if(op == '(') { // Checks if token is start of a expression
        lex_new_expression(); // Turn token type in to expression, increment expression count and create paranthases buffer
    }
    return token;
}

/*
 * Function creates and returns a new token symbol.
 * checks if symbol is ')' in that case handles expression count
 * with lex_finish_expression function.
 * */
static struct token* token_make_symbol() {
     char c = nextc(); // Get next character from file
     if(c == ')') { // If character is closing expression
         lex_finish_expression(); 
     }
     // Construct and return a new token of type symbol with a character value
     struct token* token = token_create(&(struct token){.type = TOKEN_TYPE_SYMBOL, .cval = c});
     return token;
}

/*
 * Function to construct identifier token or a keyword.
 * */
static struct token* token_make_identifier_or_keyword() {
    struct buffer* buf = buffer_create(); // Create new buffer
    char c = 0; // peekc() not needed here as it is called in macro
    LEX_GETC_IF(buf, c, (c >= 'a' && c <= 'z') // Use macro to add character if valid ascii value
            || (c >= 'A' && c <= 'Z')
            || (c >= '0' && c <= '9')
            || c == '_');

    buffer_write(buf, 0x00);

    // Check if keyword

    // Create and return new token of type identifier with string value.
    return token_create(&(struct token){.type = TOKEN_TYPE_IDENTIFIER, .sval = buffer_ptr(buf)});
}

/*
 * Function checks if next character in file stream is a special keyword token.
 * if true returns a keyword token.
 * */
struct token* read_special_token() {
    char c = peekc();
    if(isalpha(c) || c == '_') {
        return token_make_identifier_or_keyword();
    }
    return NULL;
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
        OPERATOR_CASE_EXCLUDING_DIVISION: {
            token = token_make_operator_or_string();
            break;
        }
        SYMBOL_CASE: {
            token_make_symbol();
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
            token = read_special_token(); // Creates special token with value such as '_'
            if(!token) { // If token is null throw compiler error
                compiler_error(lex_process->compiler, "Unexpected token\n");
            }
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
