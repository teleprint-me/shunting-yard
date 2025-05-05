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

#include "lexer.h"

// --- Shunting Yard Algorithm ---

TokenList* shunting_yard(const TokenList* infix);

// === Main ===

int main(void) {
    const char* sample = "(((53 + 2) - (-5 * 4)) / 5) % 100";
    printf("[DEBUG] [Sample] %s\n", sample);

    TokenList* infix = tokenizer(sample);
    // printf("[DEBUG] [Infix]\n");
    // token_list_dump(infix);

    TokenList* output_queue = token_list_create();
    TokenList* operator_stack = token_list_create();

    for (size_t i = 0; i < infix->count; i++) {
        Token* symbol = infix->tokens[i]; // shared reference (owned by infix)
        if (!symbol) {
            break;
        }
        // printf("[DEBUG] [Symbol] ");
        // token_dump(symbol);

        if (token_is_number(symbol)) {
            token_list_push(output_queue, symbol); // copy to queue
        } else if (token_is_operator(symbol)) {
            while (true) {
                Token* op = token_list_peek(operator_stack); // shared reference
                if (!token_is_operator(op) || token_is_left_paren(op)) {
                    break;
                }

                int o1 = token_precedence(symbol);
                int o2 = token_precedence(op);
                if (o2 > o1 || (o2 == o1 && token_is_left_assoc(symbol))) {
                    op = token_list_pop(operator_stack); // copy from stack
                    token_list_push(output_queue, op); // copy to queue
                    token_free(op);
                } else {
                    break;
                }
            }

            token_list_push(operator_stack, symbol); // copy to stack
        } else if (token_is_left_paren(symbol)) {
            token_list_push(operator_stack, symbol); // copy to stack
        } else if (token_is_right_paren(symbol)) {
            while (true) {
                Token* op = token_list_peek(operator_stack); // shared reference
                if (!op || token_is_left_paren(op) || token_list_is_empty(operator_stack)) {
                    break;
                }
                op = token_list_pop(operator_stack); // copy from stack
                token_list_push(output_queue, op); // copy to queue
                token_free(op); // needs to be freed
            }
            Token* op = token_list_peek(operator_stack);
            if (op && token_is_left_paren(op)) {
                Token* temp = token_list_pop(operator_stack); // copy from stack
                token_free(temp); // needs to be freed
            } else {
                /// @todo handle conversion error
            }
        }
    }

    Token* op = token_list_pop(operator_stack); // copy from stack
    while (op) {
        token_list_push(output_queue, op); // copy to queue
        token_free(op); // needs to be freed
        op = token_list_pop(operator_stack); // copy from stack
    }

    printf("[DEBUG] [Queue]\n");
    token_list_dump(output_queue);

    token_list_free(operator_stack);
    token_list_free(output_queue);
    token_list_free(infix);
    return 0;
}

// === End ===
