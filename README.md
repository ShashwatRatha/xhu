# alg

`alg` is a lightweight integer expression interpreter written in C. It supports arithmetic, logical and bitwise operations, control flow, functions, and scoped variables, all implemented through a custom lexer, parser, AST, and evaluator pipeline.

---

## Overview

The interpreter follows a classical compilation pipeline:

Source Code → Lexer → Tokens → Parser → AST → Evaluator → Result

- The lexer converts raw input into tokens.
- The parser builds an Abstract Syntax Tree (AST).
- The evaluator executes the AST using symbol and function tables.

---

## Features

- Integer arithmetic: + - * / %
- Bitwise operations: & | ^ ~ << >>
- Logical operations: && || !
- Comparisons: == != < <= > >=
- Assignment:
  = += -= *= /= %= <<= >>=
- Increment / Decrement:
  ++x  x++  --x  x--
- Control flow:
  if (cond) { ... } else { ... }
  while (cond) { ... }
  for (init; cond; update) { ... }
- Loop control:
  break
  continue
- Functions:
  func name(a, b) { ... return expr }
- Output:
  print expr
  show

---

## Project Structure

### Core Components

- Lexer: tokenizes input
- Parser: builds AST
- AST: node constructors and memory management
- Evaluator: executes AST

### Runtime Systems

- Symbol Table: variable bindings with scope
- Function Table: stores user-defined functions
- Execution Driver: REPL and file execution
- Error Handling: centralized error system

---

## Building

```bash
scripts/build

```

---

## Usage

### REPL

./alg

### Run a File

./alg program.alg

---

## Example

x = 10
y = 20

print x + y

func add(a, b) {
  return a + b
}

print add(3, 4)

for (i = 0; i < 5; i = i + 1) {
  print i
}

---

## Execution Model

- Each block introduces a new scope.
- Variables are resolved lexically.
- Functions are globally stored.
- Evaluation returns structured results.

---

## Error Handling

Errors are printed as:

alg: <message>

---

## Design Notes

- Recursive AST evaluation
- Manual memory management
- Lexical scoping via chained tables
- Operator precedence encoded in parser

---

## Possible Extensions

- Floating point support
- Arrays and data structures
- Bytecode compilation
- Static typing
