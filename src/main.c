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

// === Main ===

int main(void) {
    const char* expression = "(((53 + 2) - (-5. ** 4)) / 5) % 100";
    printf("[DEBUG] [INFIX] %s\n", expression);

    TokenList* infix = tokenizer(expression);
    printf("[DEBUG] [INFIX] %s\n", shunt_is_valid_infix(infix) ? "\u2705" : "\u274C");

    TokenList* postfix = shunt_yard(infix);
    printf("[DEBUG] [POSTFIX] %s\n", shunt_is_valid_postfix(postfix) ? "\u2705" : "\u274C");

    token_list_dump(postfix);
    token_list_free(postfix);
    token_list_free(infix);

    return 0;
}

// === End ===
