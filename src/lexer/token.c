/**
 * Copyright © 2025 Austin Berrio
 *
 * @file src/lexer/token.c
 * @brief Core Token Analysis and Classification for arithmetic expressions.
 * @note This API is kept minimal, focused, and extendable.
 * @warning If you pop it, you own it. If you push it, you clone it. If you free it, you kill it.
 */

#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "lexer/token.h"

// --- Char Types ---

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

// --- Token Precendence ---

Precedent token_precedence(const Token* token) {
    if (!token) {
        return PRECEDENCE_ERROR;
    }

    switch (token->type) {
        case TOKEN_PLUS:
        case TOKEN_MINUS:
            return PRECEDENCE_ADDITIVE;
        case TOKEN_STAR:
        case TOKEN_SLASH:
        case TOKEN_MOD:
            return PRECEDENCE_MULTIPLICATIVE;
        default:
            return PRECEDENCE_NONE;
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
    token->type = TOKEN_NONE;
    token->kind = KIND_NONE;
    token->role = ROLE_NONE;
    token->association = ASSOCIATE_NONE;
    token->precedence = PRECEDENCE_NONE;

    return token;
}

Token* token_create_number(const char* lexeme) {
    if (!lexeme) {
        return NULL;
    }

    size_t span = 0;
    const char* start = lexeme;
    bool seen_dot = false;

    while (*lexeme) {
        if (isdigit(*lexeme)) {
            span++;
        } else if (!seen_dot && *lexeme == '.') {
            seen_dot = true;
            span++;
        } else {
            break;
        }
        lexeme++;
    }

    Token* token = token_create(start, span);
    if (!token) {
        return NULL;
    }

    token->precedence = PRECEDENCE_NONE;
    token->association = ASSOCIATE_NONE;
    token->role = ROLE_NONE;
    token->kind = KIND_LITERAL;
    token->type = seen_dot ? TOKEN_FLOAT : TOKEN_INTEGER;

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
        token->type = TOKEN_NONE;
    }

    token->precedence = token_precedence(token);
    token->association = ASSOCIATE_LEFT;
    token->role = ROLE_BINARY;
    token->kind = KIND_OPERATOR;

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
        token->type = TOKEN_NONE;
    }

    token->precedence = PRECEDENCE_NONE;
    token->association = ASSOCIATE_NONE;
    token->role = ROLE_NONE;
    token->kind = KIND_GROUP;

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

    clone->precedence = token->precedence;
    clone->association = token->association;
    clone->role = token->role;
    clone->kind = token->kind;
    clone->type = token->type;

    return clone;
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
