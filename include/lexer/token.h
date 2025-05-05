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
    int precedence; // Operator precedence (-1 if not applicable)
    Associate association; // Operator associativity
    TokenRole role; // Unary, binary, etc.
    TokenKind kind; // Literal, operator, group
    TokenType type; // Specific token type
    size_t size; // Length of lexeme
    char* lexeme; // Null-terminated copy of token string
} Token;

// --- Token List ---

typedef struct TokenList {
    size_t count;
    size_t capacity;
    Token** tokens;
} TokenList;

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

bool token_is_kind(const Token* token, TokenKind kind);
bool token_is_kind_none(const Token* token);

bool token_is_type(const Token* token, TokenType type);
bool token_is_type_left_paren(const Token* token);
bool token_is_type_right_paren(const Token* token);
bool token_is_type_none(const Token* token);

bool token_is_associative(const Token* token, Associate association);
bool token_is_assoc_left(const Token* token);
bool token_is_assoc_right(const Token* token);
bool token_is_assoc_none(const Token* token);

// --- Token utilities ---

int token_precedence(const Token* token); // Internal logic for precedence table
void token_dump(const Token* token);
void token_free(Token* token);

#endif // LEXER_TOKEN_H
