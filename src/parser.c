/**
 * Copyright (C) 2025 Austin Berrio
 *
 * @file parser.c
 * @brief Implementation of the parser module for converting infix expressions to postfix
 * expressions.
 *
 * @details
 * This file contains the implementation of the `shunting_yard` function, which
 * converts an infix expression represented as a `TokenList` into a postfix expression
 * also represented as a `TokenList`. The conversion is done using the Shunting Yard algorithm.
 *
 * @note The parser assumes that the input `TokenList` is already tokenized
 * and validated. The output `TokenList` will contain tokens in postfix order.
 */

#include "lexer/token.h"
#include "lexer/token_list.h"
#include "lexer/tokenizer.h"
#include "parser.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

void shunting_set_unary_context(TokenList* list, const Token* symbol, size_t i) {
    if (!list || !list->tokens || !symbol) {
        return;
    }

    const Token* prev = (i > 0) ? token_list_peek_index(list, i - 1) : NULL;

    if (i == 0 || token_is_operator(prev) || token_is_type_left_paren(prev)) {
        ((Token*) symbol)->role = TOKEN_ROLE_UNARY;
        ((Token*) symbol)->association = TOKEN_ASSOCIATE_RIGHT;
    }
}

TokenList* shunting_yard(const TokenList* infix) {
    if (!infix || !infix->tokens || token_list_is_empty(infix)) {
        return NULL;
    }

    TokenList* output_queue = token_list_create();
    TokenList* operator_stack = token_list_create();

    for (size_t i = 0; i < infix->count; i++) {
        const Token* symbol = token_list_peek_index(infix, i);
        if (!symbol) {
            break;
        }

        if (token_is_operator(symbol)) {
            const Token* prev = token_list_peek_index(infix, i - 1);
            if (i == 0 || token_is_operator(prev) || token_is_type_left_paren(prev)) {
                ((Token*) symbol)->role = TOKEN_ROLE_UNARY;
                ((Token*) symbol)->association = TOKEN_ASSOCIATE_RIGHT;
            }
        }

        if (token_is_number(symbol)) {
            if (!token_list_push(output_queue, (Token*) symbol)) {
                fprintf(stderr, "[ERROR] Failed to push token to output queue.\n");
                goto error;
            }
        } else if (token_is_operator(symbol)) {
            while (true) {
                const Token* op = token_list_peek(operator_stack);
                if (!token_is_operator(op) || token_is_type_left_paren(op)) {
                    break;
                }

                int o1 = token_precedence(symbol);
                int o2 = token_precedence(op);
                if (o2 > o1 || (o2 == o1 && token_is_associate_left(symbol))) {
                    Token* popped = token_list_pop(operator_stack);
                    if (!token_list_push(output_queue, popped)) {
                        fprintf(stderr, "[ERROR] Failed to push token to output queue.\n");
                        token_free(popped);
                        goto error;
                    }
                    token_free(popped);
                } else {
                    break;
                }
            }

            if (!token_list_push(operator_stack, (Token*) symbol)) {
                fprintf(stderr, "[ERROR] Failed to push token to operator stack.\n");
                goto error;
            }
        } else if (token_is_type_left_paren(symbol)) {
            if (!token_list_push(operator_stack, (Token*) symbol)) {
                fprintf(stderr, "[ERROR] Failed to push token to operator stack.\n");
                goto error;
            }
        } else if (token_is_type_right_paren(symbol)) {
            while (true) {
                const Token* op = token_list_peek(operator_stack);
                if (!op || token_is_type_left_paren(op) || token_list_is_empty(operator_stack)) {
                    break;
                }

                Token* popped = token_list_pop(operator_stack);
                if (!token_list_push(output_queue, popped)) {
                    fprintf(stderr, "[ERROR] Failed to push token to output queue.\n");
                    token_free(popped);
                    goto error;
                }
                token_free(popped);
            }

            const Token* op = token_list_peek(operator_stack);
            if (op && token_is_type_left_paren(op)) {
                Token* temp = token_list_pop(operator_stack);
                token_free(temp);
            } else {
                fprintf(stderr, "[ERROR] Mismatched parentheses in column %zu\n", i);
                goto error;
            }
        }
    }

    Token* op = NULL;
    while ((op = token_list_pop(operator_stack))) {
        token_list_push(output_queue, op);
        token_free(op);
    }
    token_list_free(operator_stack);
    return output_queue;

error:
    token_list_free(output_queue);
    token_list_free(operator_stack);
    return NULL;
}
