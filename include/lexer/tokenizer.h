/**
 * @file include/lexer/tokenizer.h
 * @brief Core Token Generator used in lexical analysis.
 */

#ifndef LEXER_TOKENIZER_H
#define LEXER_TOKENIZER_H

#include "lexer/token_list.h"

TokenList* tokenizer(const char* expression);

#endif // LEXER_TOKENIZER_H
