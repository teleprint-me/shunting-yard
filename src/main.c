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

bool shunt_validate(TokenList* postfix) {
    int64_t depth = 0;

    for (size_t i = 0; i < postfix->count; i++) {
        const Token* token = token_list_peek_index(postfix, i);

        if (token_is_number(token)) {
            depth += 1;
        } else if (token_is_role_unary(token)) {
            if (depth < 1) {
                return false; // malformed
            }
            // depth unchanged
        } else if (token_is_role_binary(token)) {
            if (depth < 2) {
                return false; // malformed
            }
            depth -= 1; // two pops, one push
        } else {
            return false; // unknown token
        }
    }

    return depth == 1;
}

// === Main ===

int main(void) {
    const char* expression = "(((53 + 2) - (-5. * 4)) / 5) % 100";
    printf("[DEBUG] [INFIX] %s\n", expression);

    TokenList* infix = tokenizer(expression);
    TokenList* postfix = shunt_yard(infix);

    printf("[DEBUG] [POSTFIX] [VALID] %s\n", shunt_validate(postfix) ? "YES" : "NO");

    token_list_dump(postfix);
    token_list_free(postfix);
    token_list_free(infix);

    return 0;
}

// === End ===
