/**
 * Copyright © 2025 Austin Berrio
 *
 * @file include/lexer/token.h
 * @brief Core Token Analysis and Classification.
 * @note This API only considers arithmetic to keep it dead simple.
 * @warning If you pop it, you own it. If you push it, you clone it. If you free it, you kill it.
 */

#ifndef LEXER_TOKEN_H
#define LEXER_TOKEN_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef enum Associate {
    ASSOCIATE_NONE,
    ASSOCIATE_LEFT,
    ASSOCIATE_RIGHT,
} Associate;

typedef enum TokenRole {
    ROLE_NONE,
    ROLE_UNARY,
    ROLE_BINARY,
} TokenRole;

typedef enum TokenKind {
    KIND_NONE,
    KIND_LITERAL,
    KIND_OPERATOR,
    KIND_GROUP,
} TokenKind;

typedef enum TokenType {
    TOKEN_NONE,

    // KIND_LITERAL
    TOKEN_INTEGER,
    TOKEN_FLOAT,

    // KIND_OPERATOR
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_MOD,

    // KIND_GROUP
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
} TokenType;

typedef struct Token {
    int precedence;
    Associate association;
    TokenRole role;
    TokenKind kind;
    TokenType type;
    size_t size;
    char* lexeme;
} Token;

typedef struct TokenList {
    size_t count;
    size_t capacity;
    Token** tokens;
} TokenList;

// --- Char Types ---

bool isop(const char s);
bool isgroup(const char s);

// --- Token Precendence ---

/// @note This is a private method and is here for completeness.
int token_precedence(const Token* token);

// --- Token Operations ---

Token* token_create(const char* lexeme, const size_t size);
Token* token_create_number(const char* lexeme);
Token* token_create_operator(const char* lexeme);
Token* token_create_group(const char* lexeme);
Token* token_create_clone(const Token* token);

bool token_is_number(const Token* token);
bool token_is_operator(const Token* token);
bool token_is_group(const Token* token);

bool token_is_role(const Token* token, TokenRole role);
bool token_is_role_none(const Token* token, TokenRole role);

bool token_is_kind(const Token* token, TokenKind kind);
bool token_is_kind_none(const Token* token);

bool token_is_type(const Token* token, TokenType type);
bool token_is_type_left_paren(const Token* token);
bool token_is_type_right_paren(const Token* token);
bool token_is_type_type_none(const Token* token);

bool token_is_associative(const Token* token, Associate association);
bool token_is_assoc_left(const Token* token);
bool token_is_assoc_right(const Token* token);
bool token_is_assoc_none(const Token* token);

void token_dump(const Token* token);
void token_free(Token* token);

#endif // LEXER_TOKEN_H
