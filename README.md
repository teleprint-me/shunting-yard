# Shunting-Yard Algorithm

A minimal C implementation for converting infix arithmetic expressions to postfix (RPN).

## Overview

The **Shunting-yard algorithm**, invented by **Edsger Dijkstra**, parses arithmetic expressions
written in **infix notation** and converts them to **postfix notation** (Reverse Polish Notation, or
RPN).

Postfix form simplifies evaluation by removing the need for parentheses and explicit precedence
handling, making it ideal for calculators, interpreters, and compilers.

## Dependencies

- POSIX
- `libc`

## Clone and Build

1. **Clone the repository**

```sh
git clone https://github.com/teleprint-me/shunting-yard.git
cd shunting-yard
```

2. **Generate build files**

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Debug
```

3. **Compile**

```sh
cmake --build build --config Debug -j$(nproc)
```

4. **Run**

```sh
./build/rpn "<expression>"
```

## Scope

Currently supports basic arithmetic operations. The design prioritizes clarity and minimalism over
feature creep.

**Supported operations:**

- Addition (`+`)
- Subtraction (`-`)
- Multiplication (`*`)
- Division (`/`)
- Modulus (`%`)

**Planned features:**

- Variables
- Functions

**Production Rules:**

```ebnf
expr       → expr + term
           | expr - term
           | term

term       → term * factor
           | term / factor
           | term % factor
           | factor

factor     → unary
           | ( expr )

unary      → - unary
           | + unary
           | literal

literal    → INTEGER
           | FLOAT
```

**Details:**

- **Unary operators (`+`, `-`)**: Right-associative, allowing constructs like `--5` or `+-3.14`.
- **Literals**: Both `INTEGER` and `FLOAT` tokens are treated as terminal symbols (recognized by the
  lexer).
- **Parentheses**: Used for grouping, preserving correct precedence.

## Core Algorithm

```text
initialize empty output queue
initialize empty operator stack

for each token in input:
    if token is a number:
        add to output queue
    else if token is an operator o1:
        while operator o2 is on top of stack and not a left parenthesis:
            if o2 has greater precedence than o1
            or (equal precedence and o1 is left-associative):
                pop o2 to output queue
            else:
                break
        push o1 onto stack
    else if token is a left parenthesis:
        push onto stack
    else if token is a right parenthesis:
        while top of stack is not a left parenthesis:
            assert stack is not empty
            pop to output queue
        pop and discard left parenthesis
    else:
        handle error

while operators remain on stack:
    assert no parentheses remain
    pop to output queue

return output queue
```

## Step-by-Step Explanation

### 1. **Initialize**

- `output_queue`: stores the final postfix expression
- `operator_stack`: temporarily holds operators and parentheses

### 2. **Token Processing**

#### a. **Operands (numbers)**

- Append directly to `output_queue`

#### b. **Operators (`+`, `-`, `*`, `/`, `%`)**

- Let `o1` be the current operator
- While top of stack holds an operator `o2` (not a parenthesis):

  - If `o2` has higher precedence, or same precedence and `o1` is left-associative:

    - Pop `o2` to `output_queue`

  - Else: break

- Push `o1` onto `operator_stack`

#### c. **Left Parenthesis `(`**

- Push onto `operator_stack`

#### d. **Right Parenthesis `)`**

- Pop operators to `output_queue` until a left parenthesis is found
- Discard the left parenthesis
- Raise an error if no match is found

#### e. **Unknown Tokens**

- Handle or reject them explicitly

### 3. **Finalization**

- Pop remaining operators to `output_queue`
- Raise an error if any unmatched parentheses remain

### 4. **Result**

- `output_queue` now holds the expression in postfix form

## Notes

- **Operator precedence and associativity** are hardcoded:
  - `*`, `/`, `%` have higher precedence than `+`, `-`
  - All operators are **left-associative** (unary operators are **right-associative**)
- Input must be **tokenized before parsing**—this project separates lexing from parsing
- This tool **does not evaluate** expressions (yet); it only converts infix to postfix
- The parser assumes valid input for now; error handling is minimal and in progress

## License

Licensed under the AGPL to guarantee free use, modification, and distribution of the source code.
Primarily built for educational use, transparency, and future extensibility.
