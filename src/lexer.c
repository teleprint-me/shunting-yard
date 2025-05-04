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

    token->type = TOKEN_INTEGER;
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

    return token;
}

Token* token_clone(const Token* token) {
    if (!token) {
        return NULL;
    }

    return token_create(token->lexeme, token->size);
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

int token_precendence(const Token* token) {
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

Associate token_association(const Token* token) {
    if (!token) {
        return ASSOCIATE_INVALID;
    }

    return ASSOCIATE_LEFT;
}

void token_dump(const Token* token) {
    printf("Token: lexeme='%s', size=%zu, type=%d\n", token->lexeme, token->size, token->type);
}

void token_free(Token* token) {
    if (token) {
        assert(token->lexeme != (void*) 0xDEADC0DE && "Double free detected!\n");
        if (token->lexeme) {
            free(token->lexeme);
        }
        token->lexeme = (void*) 0xDEADC0DE;
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

    if (list->count == list->capacity) {
        size_t capacity = list->capacity * 2;
        Token** temp = realloc(list->tokens, sizeof(Token*) * capacity);
        if (!temp) {
            return false;
        }
        list->tokens = temp;
        list->capacity = capacity;
    }

    list->tokens[list->count++] = token;
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
    token_free(token);
    list->tokens[list->count - 1] = NULL;
    list->count--;
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
    token_free(token);

    // Shift elements left
    list->count--;
    for (size_t i = index; i < list->count - 1; ++i) {
        list->tokens[i] = list->tokens[i + 1];
    }

    return clone;
}

Token* token_list_peek(const TokenList* list) {
    if (!list || !list->tokens || list->count == 0) {
        return NULL;
    }

    return list->tokens[list->count - 1];
}

void token_list_dump(const TokenList* list) {
    if (!list) {
        return;
    }

    for (size_t i = 0; i < list->count; i++) {
        Token* token = list->tokens[i];
        printf(
            "Token: index=%zu, lexeme='%s', size=%zu, type=%d\n",
            i,
            token->lexeme,
            token->size,
            token->type
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
    }

    return list;
}
