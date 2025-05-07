/**
 * Copyright © 2025 Austin Berrio
 *
 * @file src/lexer/token_list.c
 * @brief Core Token Management for arithmetic expressions.
 * @note This API is kept minimal, focused, and extendable.
 * @warning Ownership Model:
 *     - If you pop it, you own it.
 *     - If you push it, you clone it.
 *     - If you free it, you kill it.
 * @ref https://www.gingerbill.org/article/2020/06/21/the-ownership-semantics-flaw/
 */

#include <stdlib.h>
#include <stdio.h>

#include "lexer/token_list.h"

TokenList* token_list_create(void) {
    TokenList* list = malloc(sizeof(TokenList));
    if (!list) {
        return NULL;
    }

    list->tokens = malloc(sizeof(Token*));
    if (!list->tokens) {
        free(list);
        return NULL;
    }

    list->capacity = 1;
    list->count = 0;
    return list;
}

void token_list_free(TokenList* list) {
    if (list) {
        if (list->tokens) {
            for (size_t i = 0; i < list->count; i++) {
                if (list->tokens[i]) {
                    token_free(list->tokens[i]);
                }
            }
            free(list->tokens);
        }
        free(list);
    }
}

bool token_list_is_empty(const TokenList* list) {
    return list->count == 0;
}

bool token_list_is_full(const TokenList* list) {
    return list->count >= list->capacity;
}

bool token_list_push(TokenList* list, Token* token) {
    if (!list || !list->tokens || !token) {
        return false;
    }

    if (token_list_is_full(list)) {
        size_t capacity = list->capacity * 2;
        Token** temp = realloc(list->tokens, sizeof(Token*) * capacity);
        if (!temp) {
            return false;
        }
        list->tokens = temp;
        list->capacity = capacity;
    }

    list->tokens[list->count++] = token_clone(token);
    return true;
}

Token* token_list_pop(TokenList* list) {
    if (!list || !list->tokens || list->count == 0) {
        return NULL;
    }

    Token* token = list->tokens[list->count - 1];
    if (!token) {
        return NULL;
    }

    Token* clone = token_clone(token);
    list->tokens[list->count - 1] = NULL;
    list->count--;
    token_free(token);
    return clone;
}

Token* token_list_pop_index(TokenList* list, int64_t index) {
    if (!list || !list->tokens || list->count == 0) {
        return NULL;
    }

    // Normalize negative index
    if (index < 0) {
        index += list->count;
    }
    if (index < 0 || (size_t) index >= list->count) {
        return NULL;
    }

    Token* token = list->tokens[(size_t) index];
    if (!token) {
        return NULL;
    }

    list->tokens[(size_t) index] = NULL;
    Token* clone = token_clone(token);

    // Shift elements left
    list->count--;
    for (size_t i = index; i < list->count - 1; ++i) {
        list->tokens[i] = list->tokens[i + 1];
    }

    token_free(token);
    return clone;
}

const Token* token_list_peek(const TokenList* list) {
    if (!list || !list->tokens || list->count == 0) {
        return NULL;
    }

    if (token_list_is_empty(list)) {
        return NULL;
    }

    return list->tokens[list->count - 1];
}

const Token* token_list_peek_index(const TokenList* list, const size_t index) {
    if (!list || !list->tokens || list->count == 0) {
        return NULL;
    }

    if (index >= list->count) {
        return NULL;
    }

    return list->tokens[index];
}

void token_list_dump(const TokenList* list) {
    if (!list) {
        return;
    }

    for (size_t i = 0; i < list->count; i++) {
        Token* token = list->tokens[i];
        printf(
            "[TokenList] index=%zu, lexeme='%s', size=%zu, type=%s, kind=%s, assoc=%s, prec=%s\n",
            i,
            token->lexeme,
            token->size,
            token_type_to_string(token),
            token_kind_to_string(token),
            token_associate_to_string(token),
            token_precedent_to_string(token)
        );
    }
}
