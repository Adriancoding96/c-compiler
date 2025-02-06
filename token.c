#include "compiler.h"

/*
 * Function checks if token is of type keyword, and compares token value
 * with input value.
 * */
bool token_is_keyword(struct token* token, const char* value) {
    return token->type == TOKEN_TYPE_KEYWORD && S_EQ(token->sval, value);
}
