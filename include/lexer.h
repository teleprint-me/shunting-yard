/**
 * Copyright © 2025 Austin Berrio
 *
 * @file include/lexer.h
 * @brief Lexical Analyzer.
 * @warning If you pop it, you own it. If you push it, you clone it. If you free it, you kill it.
 */

#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef enum TokenType {
    TOKEN_UNKNOWN,
    TOKEN_INTEGER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_MOD,
    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
} TokenType;

typedef struct Token {
    TokenType type;
    size_t size;
    char* lexeme;
} Token;

typedef struct TokenList {
    size_t count;
    size_t capacity;
    Token** tokens;
} TokenList;

typedef enum Associate {
    ASSOCIATE_INVALID,
    ASSOCIATE_LEFT,
    ASSOCIATE_RIGHT,
} Associate;

// --- Utilities ---

bool isop(const char s);
bool isgroup(const char s);

// --- Token Operations ---

Token* token_create(const char* lexeme, const size_t size);
Token* token_create_number(const char* lexeme);
Token* token_create_operator(const char* lexeme);
Token* token_create_group(const char* lexeme);
Token* token_clone(const Token* token);
bool token_is_number(const Token* token);
bool token_is_operator(const Token* token);
bool token_is_group(const Token* token);
int token_precendence(const Token* token);
Associate token_association(const Token* token);
void token_dump(const Token* token);
void token_free(Token* token);

// --- Token List Operations ---

TokenList* token_list_create(void);
bool token_list_push(TokenList* list, Token* token);
Token* token_list_pop(TokenList* list);
Token* token_list_pop_index(TokenList* list, int64_t index);
Token* token_list_peek(const TokenList* list);
void token_list_dump(const TokenList* list);
void token_list_free(TokenList* list);

// --- Lexical Analysis ---

TokenList* tokenizer(const char* expression);

#endif // LEXER_H
