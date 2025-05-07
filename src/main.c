/**
 * Copyright © 2025 Austin Berrio
 *
 * @file src/main.c
 * @brief The shunting yard algorithm completely from scratch in pure C.
 *
 * @ref https://en.wikipedia.org/wiki/Shunting_yard_algorithm
 * @ref https://mathcenter.oxford.emory.edu/site/cs171/shuntingYardAlgorithm/
 */

#include <assert.h>
#include <stdio.h>

#include "lexer/token.h"
#include "lexer/token_list.h"
#include "lexer/tokenizer.h"
#include "parser.h"

void shunt_postfix_debug(TokenList* postfix) {
    printf("[DEBUG] [POSTFIX] ");
    for (size_t i = 0; i < postfix->count; i++) {
        Token* t = postfix->tokens[i];
        printf("%s ", t->lexeme);
    }
    printf("\n");
}

// === Main ===

int main(void) {
    const char* expression = "(((53 + 2) - (-5. * 4)) / 5) % 100";
    printf("[DEBUG] [INFIX] %s\n", expression);

    TokenList* infix = tokenizer(expression);
    TokenList* postfix = shunt_yard(infix);

    shunt_postfix_debug(postfix);

    token_list_free(postfix);
    token_list_free(infix);

    return 0;
}

// === End ===
