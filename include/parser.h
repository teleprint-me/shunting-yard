/**
 * Copyright (C) 2025 Austin Berrio
 *
 * @file parser.h
 * @brief Parser module for converting infix expressions to postfix expressions.
 *
 * @details
 * This module provides the functionality to convert an infix expression
 * represented as a `TokenList` into a postfix expression also represented
 * as a `TokenList`. The conversion is done using the Shunting Yard algorithm.
 *
 * @note The parser assumes that the input `TokenList` is already tokenized
 * and validated. The output `TokenList` will contain tokens in postfix order.
 */
#ifndef PARSER_H
#define PARSER_H

#include "lexer/token_list.h"

TokenList* shunting_yard(const TokenList* infix);

#endif // PARSER_H
