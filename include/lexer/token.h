/**
 * Copyright © 2025 Austin Berrio
 *
 * @file include/lexer/token.h
 * @brief Core Token Analysis and Classification for arithmetic expressions.
 * @note This API is kept minimal, focused, and extendable.
 * @warning If you pop it, you own it. If you push it, you clone it. If you free it, you kill it.
 */

#ifndef LEXER_TOKEN_H
#define LEXER_TOKEN_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// --- Precedence ---

typedef enum Precedent {
    PRECEDENCE_ERROR = -1, // Malformed token
    PRECEDENCE_NONE = 0, // No precedence (literals, parens)
    PRECEDENCE_ADDITIVE = 1, // +, -
    PRECEDENCE_MULTIPLICATIVE = 2, // *, /, %
    // More to come: unary, function call, etc.
} Precedent;

// --- Associativity ---

typedef enum Associate {
    ASSOCIATE_NONE,
    ASSOCIATE_LEFT,
    ASSOCIATE_RIGHT,
} Associate;

// --- Role: purpose in expression ---

typedef enum TokenRole {
    ROLE_NONE,
    ROLE_UNARY,
    ROLE_BINARY,
} TokenRole;

// --- Kind: lexical category ---

typedef enum TokenKind {
    KIND_NONE,
    KIND_LITERAL,
    KIND_OPERATOR,
    KIND_GROUP,
} TokenKind;

// --- Type: concrete token type ---

typedef enum TokenType {
    TOKEN_NONE,

    // Literals
    TOKEN_INTEGER,
    TOKEN_FLOAT,

    // Operators
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_MOD,

    // Grouping
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
} TokenType;

// --- Token object ---

typedef struct Token {
    Precedent precedence; // Operator precedence (-1 if malformed)
    Associate association; // Operator associativity
    TokenRole role; // Unary, binary, etc.
    TokenKind kind; // Literal, operator, group
    TokenType type; // Specific token type
    size_t size; // Length of lexeme
    char* lexeme; // Null-terminated copy of token string
} Token;

// --- Char classification ---

bool isop(const char s);
bool isgroup(const char s);

// --- Token creation ---

Token* token_create(const char* lexeme, const size_t size);
Token* token_create_number(const char* lexeme);
Token* token_create_operator(const char* lexeme);
Token* token_create_group(const char* lexeme);
Token* token_clone(const Token* token); // Deep copy

// --- Token classification ---

bool token_is_number(const Token* token);
bool token_is_operator(const Token* token);
bool token_is_group(const Token* token);

bool token_is_role(const Token* token, TokenRole role);
bool token_is_role_none(const Token* token);
bool token_is_role_unary(const Token* token);
bool token_is_role_binary(const Token* token);

bool token_is_kind(const Token* token, TokenKind kind);
bool token_is_kind_none(const Token* token);
bool token_is_kind_literal(const Token* token);
bool token_is_kind_operator(const Token* token);
bool token_is_kind_group(const Token* token);

bool token_is_type(const Token* token, TokenType type);
bool token_is_type_none(const Token* token);
bool token_is_type_integer(const Token* token);
bool token_is_type_float(const Token* token);
bool token_is_type_plus(const Token* token);
bool token_is_type_minus(const Token* token);
bool token_is_type_star(const Token* token);
bool token_is_type_slash(const Token* token);
bool token_is_type_mod(const Token* token);
bool token_is_type_left_paren(const Token* token);
bool token_is_type_right_paren(const Token* token);

bool token_is_associative(const Token* token, Associate association);
bool token_is_assoc_left(const Token* token);
bool token_is_assoc_right(const Token* token);
bool token_is_assoc_none(const Token* token);

// --- Token utilities ---

Precedent token_precedence(const Token* token); // Internal logic table
void token_dump(const Token* token);
void token_free(Token* token);

#endif // LEXER_TOKEN_H
