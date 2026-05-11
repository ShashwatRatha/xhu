#ifndef LEXER_H_
#define LEXER_H_

#include <stddef.h>

typedef enum {
  PLUS,
  MINUS,
  MUL,
  DIV,
  MOD,
  AND,
  OR,
  NOT,
  BIT_AND,
  BIT_OR,
  BIT_XOR,
  BIT_NOT,
  LSHIFT,
  RSHIFT,
  ASSGN,
  EQ,
  NEG,
  NEQ,
  INCR,
  DECR,
  ADD_ASSGN,
  SUB_ASSGN,
  MUL_ASSGN,
  MOD_ASSIGN,
  DIV_ASSGN,
  LSHIFT_ASSGN,
  RSHIFT_ASSGN,
  LPAREN,
  RPAREN,
  LBRACE,
  RBRACE,
  LBOX,
  RBOX,
  LT,
  LET,
  GT,
  GET,
  COMMA,
  SEMICOLON,
  COLON,
  NUMBER,
  IDENT,
  KEYWORD,
  NULL_TYPE
} TokenType;

/* Keywords recognised by the lexer. */
typedef enum {
  KW_SHOW, /* "show" — print all variable bindings */
  KW_IF,
  KW_ELSE,
  KW_WHILE,
  KW_FOR,
  KW_FN,
  KW_PRINT,
  KW_BREAK,
  KW_CONTINUE,
  KW_RETURN
} KeywordType;

/* A single token.  Only one union member is valid depending on type:
 *   NUMBER  → .val
 *   IDENT   → .name  (heap-allocated, freed by freeTokens)
 *   KEYWORD → .kw
 *   others  → no payload (name is NULL)
 */
typedef struct
{
  TokenType type;
  union {
    int val;
    char *name;
    KeywordType kw;
  };
} Token;

/* Array returned by emitToks, including the NULL_TYPE sentinel. */
typedef struct
{
  size_t count;
  Token *tokens;
} TokenArr;

/* Tokenise str in-place, returning a heap-allocated TokenArr.
 * Returns {0, NULL} on error. */
TokenArr emitToks(char *str);

/* Free the token array; only IDENT token names are heap-allocated. */
void freeTokens(Token *toks);

#endif /* LEXER_H_ */
