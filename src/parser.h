#ifndef PARSER_H_
#define PARSER_H_

#include "lexer.h"
#include "symtable.h"
#include <stddef.h>

/* ── AST node types ────────────────────────────────────────────────────── */

typedef enum {
  NODE_NUM,       /* integer literal          — .ival               */
  NODE_VAR,       /* variable reference       — .name               */
  NODE_BINOP,     /* binary operation         — .op, .left, .right  */
  NODE_UNARY,     /* unary prefix (-, !)      — .op, .left          */
  NODE_PRE_INC,   /* prefix  ++x              — .left (must be VAR) */
  NODE_PRE_DEC,   /* prefix  --x              — .left (must be VAR) */
  NODE_POST_INC,  /* postfix x++              — .left (must be VAR) */
  NODE_POST_DEC,  /* postfix x--              — .left (must be VAR) */
  NODE_ASSIGN,    /* assignment stmt          — .op, .name, .right  */
  NODE_SHOW,      /* 'show' keyword           — no payload          */
  NODE_EXPR_STMT, /* bare expression stmt     — .left               */
  NODE_PROGRAM,   /* root node                — .stmts, .stmt_count */
  NODE_RETURN,
  NODE_BREAK,
  NODE_CONTINUE,
  NODE_IF,
  NODE_ELSE,
  NODE_FOR,
  NODE_WHILE,
  NODE_PRINT,
  NODE_NULL
} NodeType;

typedef struct ASTNode
{
  NodeType type;
  union {
    int val;
    char *name;
    struct
    {
      struct ASTNode **stmts;
      size_t stmtCount;
    } block;
    struct
    {
      struct ASTNode *condition;
      struct ASTNode *thenBlock;
      struct ASTNode *elseBlock;
    } ifNode;
    struct
    {
      struct ASTNode *condition;
      struct ASTNode *body;
    } whileNode;
    struct
    {
      TokenType op;
      struct ASTNode *operand;
    } standaloneNode;
    struct
    {
      TokenType op;
      char *name;
      struct ASTNode *rvalue;
    } assgnNode;
    struct
    {
      struct ASTNode *left;
      struct ASTNode *right;
      TokenType op;
    } binOp;
  };
} ASTNode;

/* ── Parser state ──────────────────────────────────────────────────────── */

/* Passed by pointer to every parse function. */
typedef struct
{
  TokenArr toks;
  size_t pos;
  SymTable *sym;
  int had_error; /* set to 1 on any parse error */
} Parser;

/* ── Inline helpers ────────────────────────────────────────────────────── */

/* Return the type of the current token without consuming it. */
static inline TokenType parserPeek(const Parser *p) {
  return p->toks.tokens[p->pos].type;
}

/* Return the type of the next token (one ahead). */
static inline TokenType parserPeek2(const Parser *p) {
  size_t next = p->pos + 1;
  if (next >= p->toks.count)
    return NULL_TYPE;
  return p->toks.tokens[next].type;
}

/* Return the current token without consuming it. */
static inline Token parserCurr(const Parser *p) {
  return p->toks.tokens[p->pos];
}

/* Return the current token and advance pos. */
static inline Token parserConsume(Parser *p) {
  Token t = p->toks.tokens[p->pos];
  if (t.type != NULL_TYPE)
    p->pos++;
  return t;
}

/* True if t is one of the assignment operators. */
static inline int isAssgnOp(TokenType t) {
  return t == ASSGN || t == ADD_ASSGN || t == SUB_ASSGN || t == MUL_ASSGN ||
         t == DIV_ASSGN || t == LSHIFT_ASSGN || t == RSHIFT_ASSGN;
}

/* ── AST constructors / destructor (defined in astdefs.c)
 * ─────────────────────────────────────── */

ASTNode *astNum(int val);
ASTNode *astVar(const char *name);
ASTNode *astBinOp(TokenType op, ASTNode *left, ASTNode *right);
ASTNode *astUnary(TokenType op, ASTNode *operand);
ASTNode *astPreIncr(ASTNode *operand);
ASTNode *astPreDecr(ASTNode *operand);
ASTNode *astPostIncr(ASTNode *operand);
ASTNode *astPostDecr(ASTNode *operand);
ASTNode *astAssgn(TokenType op, const char *name, ASTNode *right);
ASTNode *astShow(void);
ASTNode *astExprStmt(ASTNode *expr);
ASTNode *astStmts(ASTNode **stmts, size_t count);
ASTNode *astBreak();
ASTNode *astReturn(ASTNode *expr);
ASTNode *astContinue();
ASTNode *astIf(ASTNode *condition, ASTNode *thenBlock, ASTNode *elseBlock);
ASTNode *astElse(ASTNode *elseBlock);
ASTNode *astWhile(ASTNode *condition, ASTNode *body);
ASTNode *astPrint(ASTNode *expression);
void astFree(ASTNode *node);

/* ── Parse functions (defined in parser.c)
 * ───────────────────────────────────────────────────── */

/* Populate a Parser before calling parseProgram. */
void parserInit(Parser *p, TokenArr toks, SymTable *sym);

/* Entry point — call for each complete input. */
ASTNode *parseProgram(Parser *p);
ASTNode *parseBlock(Parser *p);
ASTNode *parseStmts(Parser *p);
ASTNode *parseStmt(Parser *p);
ASTNode *parseIf(Parser *p);
ASTNode *parseWhile(Parser *p);
ASTNode *parseFor(Parser *p);
ASTNode *parseFunction(Parser *p);
ASTNode *parseExpr(Parser *p);
ASTNode *parseLogicOr(Parser *p);
ASTNode *parseLogicAnd(Parser *p);
ASTNode *parseBitOr(Parser *p);
ASTNode *parseBitXor(Parser *p);
ASTNode *parseBitAnd(Parser *p);
ASTNode *parseEquality(Parser *p);
ASTNode *parseShift(Parser *p);
ASTNode *parseAdditive(Parser *p);
ASTNode *parseTerm(Parser *p);
ASTNode *parseUnary(Parser *p);
ASTNode *parsePostfix(Parser *p);
ASTNode *parsePrimary(Parser *p);

#endif /* PARSER_H_ */
