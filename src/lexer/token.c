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

// --- ASCII Character Classification ---

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

// --- Token Precedent Classification ---

Precedent token_precedence(const Token* token) {
    if (!token) {
        return TOKEN_PRECEDENT_ERROR;
    }

    switch (token->type) {
        case TOKEN_TYPE_PLUS:
        case TOKEN_TYPE_MINUS:
            return TOKEN_PRECEDENT_ADDITIVE;
        case TOKEN_TYPE_STAR:
        case TOKEN_TYPE_SLASH:
        case TOKEN_TYPE_MOD:
            return TOKEN_PRECEDENT_MULTIPLICATIVE;
        default:
            return TOKEN_PRECEDENT_NONE;
    }
}

// --- Token Lifecycle Management ---

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
    token->type = TOKEN_TYPE_NONE;
    token->kind = TOKEN_KIND_NONE;
    token->role = TOKEN_ROLE_NONE;
    token->association = TOKEN_ASSOCIATE_NONE;
    token->precedence = TOKEN_PRECEDENT_NONE;

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

    token->precedence = TOKEN_PRECEDENT_NONE;
    token->association = TOKEN_ASSOCIATE_NONE;
    token->role = TOKEN_ROLE_NONE;
    token->kind = TOKEN_KIND_LITERAL;
    token->type = seen_dot ? TOKEN_TYPE_FLOAT : TOKEN_TYPE_INTEGER;

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
        token->type = TOKEN_TYPE_PLUS;
    } else if (*lexeme == '-') {
        token->type = TOKEN_TYPE_MINUS;
    } else if (*lexeme == '*') {
        token->type = TOKEN_TYPE_STAR;
    } else if (*lexeme == '/') {
        token->type = TOKEN_TYPE_SLASH;
    } else if (*lexeme == '%') {
        token->type = TOKEN_TYPE_MOD;
    } else {
        token->type = TOKEN_TYPE_NONE;
    }

    token->precedence = token_precedence(token);
    token->association = TOKEN_ASSOCIATE_LEFT;
    token->role = TOKEN_ROLE_BINARY;
    token->kind = TOKEN_KIND_OPERATOR;

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
        token->type = TOKEN_TYPE_LEFT_PAREN;
    } else if (*lexeme == ')') {
        token->type = TOKEN_TYPE_RIGHT_PAREN;
    } else {
        token->type = TOKEN_TYPE_NONE;
    }

    token->precedence = TOKEN_PRECEDENT_NONE;
    token->association = TOKEN_ASSOCIATE_NONE;
    token->role = TOKEN_ROLE_NONE;
    token->kind = TOKEN_KIND_GROUP;

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

void token_free(Token* token) {
    if (token) {
        if (token->lexeme) {
            free(token->lexeme);
            token->lexeme = NULL;
        }
        free(token);
    }
}

// --- Token Classification ---

bool token_is_number(const Token* token) {
    if (!token) {
        return false;
    }

    switch (token->type) {
        case TOKEN_TYPE_INTEGER:
        case TOKEN_TYPE_FLOAT:
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
        case TOKEN_TYPE_PLUS:
        case TOKEN_TYPE_MINUS:
        case TOKEN_TYPE_STAR:
        case TOKEN_TYPE_SLASH:
        case TOKEN_TYPE_MOD:
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
        case TOKEN_TYPE_LEFT_PAREN:
        case TOKEN_TYPE_RIGHT_PAREN:
            return true;
        default:
            return false;
    }
}

// --- Token Role Classification ---

bool token_is_role(const Token* token, TokenRole role) {
    return token && token->role == role;
}

bool token_is_role_none(const Token* token) {
    return token_is_role(token, TOKEN_ROLE_NONE);
}

bool token_is_role_unary(const Token* token) {
    return token_is_role(token, TOKEN_ROLE_UNARY);
}

bool token_is_role_binary(const Token* token) {
    return token_is_role(token, TOKEN_ROLE_BINARY);
}

// --- Token Kind Classification ---

bool token_is_kind(const Token* token, TokenKind kind) {
    return token && token->kind == kind;
}

bool token_is_kind_none(const Token* token) {
    return token_is_kind(token, TOKEN_KIND_NONE);
}

bool token_is_kind_literal(const Token* token) {
    return token_is_kind(token, TOKEN_KIND_LITERAL);
}

bool token_is_kind_operator(const Token* token) {
    return token_is_kind(token, TOKEN_KIND_OPERATOR);
}

bool token_is_kind_group(const Token* token) {
    return token_is_kind(token, TOKEN_KIND_GROUP);
}

// --- Token Type Classification ---

bool token_is_type(const Token* token, TokenType type) {
    return token && token->type == type;
}

bool token_is_type_none(const Token* token) {
    return token_is_type(token, TOKEN_TYPE_NONE);
}

bool token_is_type_integer(const Token* token) {
    return token_is_type(token, TOKEN_TYPE_INTEGER);
}

bool token_is_type_float(const Token* token) {
    return token_is_type(token, TOKEN_TYPE_FLOAT);
}

bool token_is_type_plus(const Token* token) {
    return token_is_type(token, TOKEN_TYPE_PLUS);
}

bool token_is_type_minus(const Token* token) {
    return token_is_type(token, TOKEN_TYPE_MINUS);
}

bool token_is_type_star(const Token* token) {
    return token_is_type(token, TOKEN_TYPE_STAR);
}

bool token_is_type_slash(const Token* token) {
    return token_is_type(token, TOKEN_TYPE_SLASH);
}

bool token_is_type_mod(const Token* token) {
    return token_is_type(token, TOKEN_TYPE_MOD);
}

bool token_is_type_left_paren(const Token* token) {
    return token_is_type(token, TOKEN_TYPE_LEFT_PAREN);
}

bool token_is_type_right_paren(const Token* token) {
    return token_is_type(token, TOKEN_TYPE_RIGHT_PAREN);
}

// --- Token Associativity Classification ---

bool token_is_associate(const Token* token, Associate association) {
    return token && token->association == association;
}

bool token_is_associate_none(const Token* token) {
    return token_is_associate(token, TOKEN_ASSOCIATE_NONE);
}

bool token_is_associate_left(const Token* token) {
    return token_is_associate(token, TOKEN_ASSOCIATE_LEFT);
}

bool token_is_associate_right(const Token* token) {
    return token_is_associate(token, TOKEN_ASSOCIATE_RIGHT);
}

// --- Token to String Conversion Functions ---

const char* token_type_to_string(const Token* token) {
    if (!token) {
        return "NULL";
    }
    switch (token->type) {
        case TOKEN_TYPE_NONE:
            return "NONE";
        case TOKEN_TYPE_INTEGER:
            return "INTEGER";
        case TOKEN_TYPE_FLOAT:
           return "FLOAT";
        case TOKEN_TYPE_PLUS:
            return "PLUS";
        case TOKEN_TYPE_MINUS:
            return "MINUS";
        case TOKEN_TYPE_STAR:
            return "STAR";
        case TOKEN_TYPE_SLASH:
            return "SLASH";
        case TOKEN_TYPE_MOD:
            return "MOD";
        case TOKEN_TYPE_LEFT_PAREN:
            return "LEFT_PAREN";
        case TOKEN_TYPE_RIGHT_PAREN:
            return "RIGHT_PAREN";
        default:
            return "UNKNOWN";
    }
}

const char* token_kind_to_string(const Token* token) {
    if (!token) {
        return "NULL";
    }
    switch (token->kind) {
        case TOKEN_KIND_NONE:
            return "NONE";
        case TOKEN_KIND_LITERAL:
            return "LITERAL";
        case TOKEN_KIND_OPERATOR:
            return "OPERATOR";
        case TOKEN_KIND_GROUP:
            return "GROUP";
        default:
            return "UNKNOWN";
    }
}

const char* token_associate_to_string(const Token* token) {
    if (!token) {
        return "NULL";
    }
    switch (token->association) {
        case TOKEN_ASSOCIATE_NONE:
            return "NONE";
        case TOKEN_ASSOCIATE_LEFT:
            return "LEFT";
        case TOKEN_ASSOCIATE_RIGHT:
            return "RIGHT";
        default:
            return "UNKNOWN";
    }
}

const char* token_precedent_to_string(const Token* token) {
    if (!token) {
        return "NULL";
    }
    switch (token->precedence) {
        case TOKEN_PRECEDENT_ERROR:
            return "ERROR";
        case TOKEN_PRECEDENT_NONE:
            return "NONE";
        case TOKEN_PRECEDENT_ADDITIVE:
            return "ADDITION";
        case TOKEN_PRECEDENT_MULTIPLICATIVE:
            return "MULTIPLICATIVE";
        default:
            return "UNKNOWN";
    }
}

void token_dump(const Token* token) {
    if (!token) {
        return;
    }
    printf(
        "[Token] lexeme='%s', size=%zu, type=%s, kind=%s, assoc=%s, prec=%s\n",
        token->lexeme,
        token->size,
        token_type_to_string(token),
        token_kind_to_string(token),
        token_associate_to_string(token),
        token_precedent_to_string(token)
    );
}
