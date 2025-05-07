/**
 * Copyright © 2025 Austin Berrio
 *
 * @file include/lexer/token.h
 * @brief Core Token Analysis and Classification for arithmetic expressions.
 * @note This API is kept minimal, focused, and extendable.
 */

#ifndef LEXER_TOKEN_H
#define LEXER_TOKEN_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// --- Precedence ---

typedef enum Precedent {
    TOKEN_PRECEDENT_ERROR = -1, // Malformed token
    TOKEN_PRECEDENT_NONE = 0, // No precedence (literals, parens)
    TOKEN_PRECEDENT_ADDITIVE = 1, // +, -
    TOKEN_PRECEDENT_MULTIPLICATIVE = 2, // *, /, %
    // More to come: unary, function call, etc.
} Precedent;

// --- Associativity ---

typedef enum Associate {
    TOKEN_ASSOCIATE_NONE,
    TOKEN_ASSOCIATE_LEFT,
    TOKEN_ASSOCIATE_RIGHT,
} Associate;

// --- Role: purpose in expression ---

typedef enum TokenRole {
    TOKEN_ROLE_NONE,
    TOKEN_ROLE_UNARY,
    TOKEN_ROLE_BINARY,
} TokenRole;

// --- Kind: lexical category ---

typedef enum TokenKind {
    TOKEN_KIND_NONE,
    TOKEN_KIND_LITERAL,
    TOKEN_KIND_OPERATOR,
    TOKEN_KIND_GROUP,
} TokenKind;

// --- Type: concrete token type ---

typedef enum TokenType {
    TOKEN_TYPE_NONE,

    // Literals
    TOKEN_TYPE_INTEGER,
    TOKEN_TYPE_FLOAT,

    // Operators
    TOKEN_TYPE_PLUS,
    TOKEN_TYPE_MINUS,
    TOKEN_TYPE_STAR,
    TOKEN_TYPE_SLASH,
    TOKEN_TYPE_MOD,

    // Grouping
    TOKEN_TYPE_LEFT_PAREN,
    TOKEN_TYPE_RIGHT_PAREN,
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

// --- ASCII Character Classification ---

bool isop(const char s);
bool isgroup(const char s);

// --- Token Precedent Classification ---

Precedent token_precedence(const Token* token); // Internal logic table

// --- Token Lifecycle Management ---

Token* token_create(const char* lexeme, const size_t size);
Token* token_create_number(const char* lexeme);
Token* token_create_operator(const char* lexeme);
Token* token_create_group(const char* lexeme);
Token* token_clone(const Token* token); // Deep copy
void token_free(Token* token);

// --- Token Classification ---

bool token_is_number(const Token* token);
bool token_is_operator(const Token* token);
bool token_is_group(const Token* token);

// --- Token Role Classification ---

bool token_is_role(const Token* token, TokenRole role);
bool token_is_role_none(const Token* token);
bool token_is_role_unary(const Token* token);
bool token_is_role_binary(const Token* token);

// --- Token Kind Classification ---

bool token_is_kind(const Token* token, TokenKind kind);
bool token_is_kind_none(const Token* token);
bool token_is_kind_literal(const Token* token);
bool token_is_kind_operator(const Token* token);
bool token_is_kind_group(const Token* token);

// --- Token Type Classification ---

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

// --- Token Associativity Classification ---

bool token_is_associate(const Token* token, Associate association);
bool token_is_associate_none(const Token* token);
bool token_is_associate_left(const Token* token);
bool token_is_associate_right(const Token* token);

// --- Token to String Conversion Functions ---

const char* token_type_to_string(const Token* token);
const char* token_kind_to_string(const Token* token);
const char* token_role_to_string(const Token* token);
const char* token_associate_to_string(const Token* token);
const char* token_precedent_to_string(const Token* token);
void token_dump(const Token* token);

#endif // LEXER_TOKEN_H
