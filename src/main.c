/**
 * Copyright © 2025 Austin Berrio
 *
 * @file src/main.c
 * @brief The shunting yard algorithm completely from scratch in pure C.
 *
 * @ref https://en.wikipedia.org/wiki/Shunting_yard_algorithm
 * @ref https://mathcenter.oxford.emory.edu/site/cs171/shuntingYardAlgorithm/
 */

#include "lexer.h"

// --- Shunting Yard Algorithm ---

TokenList* shunting_yard(const TokenList* infix);

// === Main ===

int main(void) {
    const char* sample = "(((53 + 2) - (-5 * 4)) / 5) % 100";
    TokenList* infix = tokenizer(sample);

    TokenList* output_queue = token_list_create();
    TokenList* operator_stack = token_list_create();

    for (size_t i = 0; i < infix->count; i++) {
        Token* symbol = token_clone(infix->tokens[i]);

        if (token_is_number(symbol)) {
            token_list_push(output_queue, symbol);
        } else if (token_is_operator(symbol)) {
            while (true) {
                Token* token = token_list_peek(operator_stack);
                if (!token_is_operator(token) || token->type == TOKEN_LEFT_PAREN) {
                    break;
                }

                int o1 = token_precendence(symbol);
                int o2 = token_precendence(token);
                Associate association = token_association(symbol);
                if (o2 > o1 || (o2 == o1 && association == ASSOCIATE_LEFT)) {
                    Token* op = token_list_pop(operator_stack);
                    token_list_push(output_queue, op);
                } else {
                    break;
                }
            }

            token_list_push(operator_stack, symbol);
        } else if (symbol->type == TOKEN_LEFT_PAREN) {
            token_list_push(operator_stack, symbol);
        } else if (symbol->type == TOKEN_RIGHT_PAREN) {
            while (true) {
                Token* token = token_list_peek(operator_stack);
                if (!token || token->type == TOKEN_LEFT_PAREN || operator_stack->count == 0) {
                    break;
                }
                token = token_list_pop(operator_stack);
                token_list_push(output_queue, token);
            }
            Token* op = token_list_peek(operator_stack);
            if (op && op->type == TOKEN_LEFT_PAREN) {
                token_list_pop(operator_stack);
            }
        } else {
            /// @todo handle conversion error
            token_free(symbol);
        }
    }

    Token* operator= token_list_pop(operator_stack);
    while (operator) {
        token_list_push(output_queue, operator);
        operator= token_list_pop(operator_stack);
    }

    token_list_free(operator_stack);
    token_list_free(output_queue);
    token_list_free(infix);
    return 0;
}

// === End ===
