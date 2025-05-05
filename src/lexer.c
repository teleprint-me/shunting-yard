/**
 * Copyright © 2025 Austin Berrio
 *
 * @file include/lexer.h
 * @brief Lexical Analyzer.
 */

#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "lexer.h"

// --- Utilities ---

bool isop(const char s) {
    switch (s) {
        case '+': // addition
        case '-': // subtraction
        case '*': // multiplication
        case '/': // division
        case '%': // modulus
            return true;
        default:
            return false;
    }
}

bool isgroup(const char s) {
    switch (s) {
        case '(':
        case ')':
            return true;
        default:
            return false;
    }
}

// --- Token Operations ---

Token* token_create(const char* lexeme, const size_t size) {
    if (!lexeme) {
        return NULL;
    }

    Token* token = malloc(sizeof(Token));
    if (!token) {
        return NULL;
    }

    token->lexeme = strndup(lexeme, size);
    if (!token->lexeme) {
        free(token);
        return NULL;
    }

    token->size = strlen(token->lexeme);
    token->type = TOKEN_UNKNOWN;
    return token;
}

Token* token_create_number(const char* lexeme) {
    size_t span = 0;
    const char* start = lexeme;
    while (*lexeme && isdigit(*lexeme)) {
        span++;
        lexeme++;
    }

    Token* token = token_create(start, span);
    if (!token) {
        return NULL;
    }

    token->kind = KIND_NUMBER;
    token->type = TOKEN_INTEGER;
    token->association = ASSOCIATE_NONE;
    token->precedence = -1;
    return token;
}

Token* token_create_operator(const char* lexeme) {
    if (!isop(*lexeme)) {
        return NULL;
    }

    Token* token = token_create(lexeme, 1);
    if (!token) {
        return NULL;
    }

    if (*lexeme == '+') {
        token->type = TOKEN_PLUS;
    } else if (*lexeme == '-') {
        token->type = TOKEN_MINUS;
    } else if (*lexeme == '*') {
        token->type = TOKEN_STAR;
    } else if (*lexeme == '/') {
        token->type = TOKEN_SLASH;
    } else if (*lexeme == '%') {
        token->type = TOKEN_MOD;
    } else {
        token->type = TOKEN_UNKNOWN;
    }

    token->kind = KIND_OPERATOR;
    token->association = ASSOCIATE_LEFT;
    token->precedence = token_precedence(token);
    return token;
}

Token* token_create_group(const char* lexeme) {
    if (!isgroup(*lexeme)) {
        return NULL;
    }

    Token* token = token_create(lexeme, 1);
    if (!token) {
        return NULL;
    }

    if (*lexeme == '(') {
        token->type = TOKEN_LEFT_PAREN;
    } else if (*lexeme == ')') {
        token->type = TOKEN_RIGHT_PAREN;
    } else {
        token->type = TOKEN_UNKNOWN;
    }

    token->kind = KIND_GROUP;
    token->association = ASSOCIATE_NONE;
    token->precedence = -1;
    return token;
}

Token* token_clone(const Token* token) {
    if (!token) {
        return NULL;
    }

    Token* clone = token_create(token->lexeme, token->size);
    if (!clone) {
        return NULL;
    }

    clone->type = token->type;
    clone->kind = token->kind;
    clone->association = token->association;
    clone->precedence = token->precedence;
    return clone;
}

int token_precedence(const Token* token) {
    if (!token) {
        return -1;
    }

    switch (token->type) {
        case TOKEN_PLUS:
        case TOKEN_MINUS:
            return 1;
        case TOKEN_STAR:
        case TOKEN_SLASH:
        case TOKEN_MOD:
            return 2;
        default:
            return -1;
    }
}

bool token_is_number(const Token* token) {
    if (!token) {
        return false;
    }

    switch (token->type) {
        case TOKEN_INTEGER:
            return true;
        default:
            return false;
    }
}

bool token_is_operator(const Token* token) {
    if (!token) {
        return false;
    }

    switch (token->type) {
        case TOKEN_PLUS:
        case TOKEN_MINUS:
        case TOKEN_STAR:
        case TOKEN_SLASH:
        case TOKEN_MOD:
            return true;
        default:
            return false;
    }
}

bool token_is_group(const Token* token) {
    if (!token) {
        return false;
    }

    switch (token->type) {
        case TOKEN_LEFT_PAREN:
        case TOKEN_RIGHT_PAREN:
            return true;
        default:
            return false;
    }
}

bool token_is_kind(const Token* token, TokenKind kind) {
    return token && token->kind == kind;
}

bool token_is_type(const Token* token, TokenType type) {
    return token && token->type == type;
}

bool token_is_left_paren(const Token* token) {
    return token_is_type(token, TOKEN_LEFT_PAREN);
}

bool token_is_right_paren(const Token* token) {
    return token_is_type(token, TOKEN_RIGHT_PAREN);
}

bool token_is_associative(const Token* token, Associate association) {
    return token && token->association == association;
}

bool token_is_left_assoc(const Token* token) {
    return token_is_associative(token, ASSOCIATE_LEFT);
}

bool token_is_right_assoc(const Token* token) {
    return token_is_associative(token, ASSOCIATE_RIGHT);
}

bool token_is_none_assoc(const Token* token) {
    return token_is_associative(token, ASSOCIATE_NONE);
}

void token_dump(const Token* token) {
    printf(
        "[Token] lexeme='%s', size=%zu, type=%d, kind=%d, assoc=%d, prec=%d\n",
        token->lexeme,
        token->size,
        token->type,
        token->kind,
        token->association,
        token->precedence
    );
}

void token_free(Token* token) {
    if (token) {
        if (token->lexeme) {
            free(token->lexeme);
            token->lexeme = NULL;
        }
        free(token);
    }
}

// --- Token List Operations ---

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

bool token_list_push(TokenList* list, Token* token) {
    if (!list || !list->tokens || !token) {
        return false;
    }

    if (list->count >= list->capacity) {
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

Token* token_list_peek(const TokenList* list) {
    if (!list || !list->tokens || list->count == 0) {
        return NULL;
    }

    return list->tokens[list->count - 1];
}

bool token_list_is_empty(const TokenList* list) {
    return list->count == 0;
}

void token_list_dump(const TokenList* list) {
    if (!list) {
        return;
    }

    for (size_t i = 0; i < list->count; i++) {
        Token* token = list->tokens[i];
        printf(
            "[TokenList] index=%zu, lexeme='%s', size=%zu, type=%d, kind=%d, assoc=%d, prec=%d\n",
            i,
            token->lexeme,
            token->size,
            token->type,
            token->kind,
            token->association,
            token->precedence
        );
    }
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

// --- Lexical Analysis ---

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
        } else {
            expression++; // ignore all whitespace
            continue;
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
        token_free(token);
    }

    return list;
}
