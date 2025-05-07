/**
 * Copyright (C) 2025 Austin Berrio
 *
 * @file include/parser.h
 * @brief Parser module for converting infix expressions to postfix expressions.
 */

#ifndef SHUNTING_YARD_H
#define SHUNTING_YARD_H

#include "lexer/token_list.h"

TokenList* shunt_yard(const TokenList* infix);

#endif // SHUNTING_YARD_H
