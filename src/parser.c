/**
 * Copyright (C) 2025 Austin Berrio
 *
 * @file src/parser.c
 * @brief Implementation of the parser module for converting infix expressions to postfix
 * expressions.
 */

#include "lexer/token.h"
#include "lexer/token_list.h"
#include "lexer/tokenizer.h"
#include "parser.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

static void shunt_unary(const TokenList* infix, const Token* symbol, size_t i) {
    if (!infix || !infix->tokens || !symbol) {
        return;
    }

    const Token* prev = (i > 0) ? token_list_peek_index(infix, i - 1) : NULL;

    if (i == 0 || token_is_operator(prev) || token_is_type_left_paren(prev)) {
        Token* editable = (Token*) symbol;
        editable->role = TOKEN_ROLE_UNARY;
        editable->association = TOKEN_ASSOCIATE_RIGHT;
        editable->precedence = TOKEN_PRECEDENT_UNARY;
    }
}

static bool shunt_precedent(TokenList* postfix, TokenList* operators, const Token* symbol) {
    while (true) {
        const Token* op = token_list_peek(operators);
        if (!token_is_operator(op) || token_is_type_left_paren(op)) {
            break;
        }

        int o1 = token_precedence(symbol);
        int o2 = token_precedence(op);
        if (o2 > o1 || (o2 == o1 && token_is_associate_left(symbol))) {
            Token* popped = token_list_pop(operators);
            if (!token_list_push(postfix, popped)) {
                token_free(popped);
                return false;
            }
            token_free(popped);
        } else {
            break;
        }
    }

    return true;
}

static bool shunt_group(TokenList* postfix, TokenList* operators, size_t i) {
    while (true) {
        const Token* op = token_list_peek(operators);
        if (!op || token_is_type_left_paren(op) || token_list_is_empty(operators)) {
            break;
        }

        Token* popped = token_list_pop(operators);
        if (!token_list_push(postfix, popped)) {
            token_free(popped);
            return false;
        }
        token_free(popped);
    }
    
    const Token* op = token_list_peek(operators);
    if (op && token_is_type_left_paren(op)) {
        Token* temp = token_list_pop(operators);
        token_free(temp);
    } else {
        fprintf(stderr, "[ERROR] Mismatched parentheses in column %zu\n", i);
        return false;
    }

    return true;
}

TokenList* shunt_yard(const TokenList* infix) {
    if (!infix || !infix->tokens || token_list_is_empty(infix)) {
        return NULL;
    }

    TokenList* postfix = token_list_create();
    TokenList* operators = token_list_create();

    for (size_t i = 0; i < infix->count; i++) {
        const Token* symbol = token_list_peek_index(infix, i);
        if (!symbol) {
            break; // Out-of-bounds
        }

        if (token_is_operator(symbol)) {
            shunt_unary(infix, symbol, i);
        }

        if (token_is_number(symbol)) {
            if (!token_list_push(postfix, (Token*) symbol)) {
                fprintf(stderr, "[ERROR] Failed to push token to output queue.\n");
                goto error;
            }
        } else if (token_is_operator(symbol)) {
            if (!shunt_precedent(postfix, operators, symbol)) {
                fprintf(stderr, "[ERROR] Failed during precedence resolution.\n");
                goto error;
            }

            if (!token_list_push(operators, (Token*) symbol)) {
                fprintf(stderr, "[ERROR] Failed to push token to operator stack.\n");
                goto error;
            }
        } else if (token_is_type_left_paren(symbol)) {
            if (!token_list_push(operators, (Token*) symbol)) {
                fprintf(stderr, "[ERROR] Failed to push token to operator stack.\n");
                goto error;
            }
        } else if (token_is_type_right_paren(symbol)) {
            if (!shunt_group(postfix, operators, i)) {
                fprintf(stderr, "[ERROR] Failed to push token to operator stack.\n");
                goto error;
            }
        }
    }

    Token* op = NULL;
    while ((op = token_list_pop(operators))) {
        if (!token_list_push(postfix, op)) {
            fprintf(stderr, "[ERROR] Failed to push operator to stack.\n");
            goto error;
        }
        token_free(op);
    }

    token_list_free(operators);
    return postfix;

error:
    token_list_free(postfix);
    token_list_free(operators);
    return NULL;
}
