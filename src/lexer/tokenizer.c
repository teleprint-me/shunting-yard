/**
 * @file src/lexer/tokenizer.c
 * @brief Core Token Generator used in lexical analysis.
 */

#include <ctype.h>

#include "lexer/tokenizer.h"

TokenList* tokenizer(const char* expression) {
    TokenList* list = token_list_create();

    while (*expression) {
        Token* token = NULL;

        if (isdigit(*expression)) {
            token = token_create_number(expression);
        } else if (isop(*expression)) {
            token = token_create_operator(expression);
        } else if (isgroup(*expression)) {
            token = token_create_group(expression);
        } else if (isspace(*expression)) {
            expression++; // ignore all whitespace
            continue;
        } else {
            token_list_free(list); // unknown character encountered
            return NULL;
        }

        if (!token) {
            token_list_free(list);
            return NULL;
        }

        if (!token_list_push(list, token)) {
            token_list_free(list);
            return NULL;
        }

        expression += token->size; // advance the stream
        token_free(token); // free the cloned token after pushing it to the list
    }

    return list;
}
