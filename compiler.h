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

    // Enum defining the diferent kinds of tokens
    enum {
        TOKEN_TYPE_IDENTIFIER,
        TOKEN_TYPE_KEYWORD,
        TOKEN_TYPE_OPERATOR,
        TOKEN_TYPE_SYMBOL,
        TOKEN_TYPE_NUMBER,
        TOKEN_TYPE_STRING,
        TOKEN_TYPE_COMMENT,
        TOKEN_TYPE_NEWLINE
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

struct lex_process;

// Function pointers, allowing abstraction, mimicing something like inheritance present oop languages
typedef char(*LEX_PROCESS_NEXT_CHAR)(struct lex_process* process);
typedef char(*LEX_PROCESS_PEEK_CHAR)(struct lex_process* process);
typedef void(*LEX_PROCESS_PUSH_CHAR)(struct lex_process* process, char c);
struct lex_process_functions {

    LEX_PROCESS_NEXT_CHAR next_char;
    LEX_PROCESS_PEEK_CHAR peek_char;
    LEX_PROCESS_PUSH_CHAR push_char;

};

struct lex_process {
    struct pos pos;
    struct vector* token_vec;
    struct compile_process* compiler;

    // Count of current number of brackets, ex - ((50)) = 2.
    int current_expression_count;
    struct buffer* parantheses_buffer;
    struct lex_process_functions* function;

    // Data that the lexer does not understand, but the user does.
    void* private;
};


// Struct defining the compile process
struct compile_process {
    // Flags for how file should be compiled
    int flags; 

    struct pos pos;
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
