/**
 * Copyright © 2025 Austin Berrio
 *
 * @file include/lexer/token_list.h
 * @brief Core Token Management for arithmetic expressions.
 * @note This API is kept minimal, focused, and extendable.
 * @warning Ownership Model:
 *     - If you pop it, you own it.
 *     - If you push it, you clone it.
 *     - If you free it, you kill it.
 * @ref https://www.gingerbill.org/article/2020/06/21/the-ownership-semantics-flaw/
 */

#ifndef LEXER_TOKEN_LIST_H
#define LEXER_TOKEN_LIST_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "lexer/token.h"

// --- Token List ---

typedef struct TokenList {
    size_t count;
    size_t capacity;
    Token** tokens;
} TokenList;

// --- Token List Operations ---

TokenList* token_list_create(void);
void token_list_free(TokenList* list);

bool token_list_is_empty(const TokenList* list);
bool token_list_is_full(const TokenList* list);

/// @warning The token is cloned into the list and is owned by the list.
bool token_list_push(TokenList* list, const Token* token);

/// @warning The token is removed and freed from the list. A copy of the token is returned.
Token* token_list_pop(TokenList* list);
Token* token_list_pop_index(TokenList* list, int64_t index);

/// @warning The returned token is owned by the list. Do not free or modify.
const Token* token_list_peek(const TokenList* list);
const Token* token_list_peek_index(const TokenList* list, const size_t index);

void token_list_dump(const TokenList* list);

#endif // LEXER_TOKEN_LIST_H
