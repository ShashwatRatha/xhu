#include "parser.h"
#include "lexer.h"
#include <stddef.h>
#include <stdlib.h>

#define STMTS_INIT 8

void parserInit(Parser *p, TokenArr toks, SymTable *sym) {
  p->toks = toks;
  p->sym = sym;
  p->pos = 0;
  p->had_error = 0;
}

// program ::= stmts
// stmts ::= stmt stmts | NULL
ASTNode *parseProgram(Parser *p) {
  ASTNode **stmts = malloc(STMTS_INIT * sizeof(*stmts));
  if (!stmts)
    return NULL;

  size_t stmtIdx = 0, stmtCap = STMTS_INIT;

  while (parserPeek(p) != NULL_TYPE) {
    ASTNode *stmt = parseStmt(p);
    if (!stmt)
      break;

    if (stmtIdx >= stmtCap) {
      stmtCap <<= 1;
      ASTNode **tmp = realloc(stmts, stmtCap * sizeof(*stmts));
      if (!tmp) {
        for (size_t i = 0; i < stmtIdx; i++)
          astFree(stmts[i]);
        free(stmts);
        return NULL;
      }
      stmts = tmp;
    }

    stmts[stmtIdx++] = stmt;
  }

  return astProgram(stmts, stmtIdx);
}

// stmt ::= show | ident assgn expr | expr
ASTNode *parseStmt(Parser *p) {
  switch (parserPeek(p)) {
  case KEYWORD: {
    parserConsume(p);
    return astShow();
  }
  case IDENT: {
    if (isAssgnOp(parserPeek2(p))) {
      Token t = parserConsume(p);
      char *name = t.name;
      TokenType op = parserConsume(p).type;
      ASTNode *expr = parseExpr(p);

      return astAssgn(op, name, expr);
    } else {
      return astExprStmt(parseExpr(p));
    }
  }
  default: {
    return astExprStmt(parseExpr(p));
  }
  } // end of switch
}

// expr ::= logic_or
ASTNode *parseExpr(Parser *p) { return parseLogicOr(p); }

// logic_or ::= logic_and | logic_and '||' logic_or
ASTNode *parseLogicOr(Parser *p) {
  ASTNode *left = parseLogicAnd(p);
  if (parserPeek(p) == OR) {
    parserConsume(p);
    return astBinOp(OR, left, parseLogicOr(p));
  }

  return left;
}

// logic_and ::= bit_or | bit_or '&&' logic_and
ASTNode *parseLogicAnd(Parser *p) {
  ASTNode *left = parseBitOr(p);
  if (parserPeek(p) == AND) {
    parserConsume(p);
    return astBinOp(AND, left, parseLogicAnd(p));
  }

  return left;
}

// bit_or ::= bit_xor | bit_xor '|' bit_or
ASTNode *parseBitOr(Parser *p) {
  ASTNode *left = parseBitXor(p);
  if (parserPeek(p) == BIT_OR) {
    parserConsume(p);
    return astBinOp(BIT_OR, left, parseBitOr(p));
  }

  return left;
}

// bit_xor ::= bit_and | bit_and '^' bit_xor
ASTNode *parseBitXor(Parser *p) {
  ASTNode *left = parseBitAnd(p);
  if (parserPeek(p) == BIT_XOR) {
    parserConsume(p);
    return astBinOp(BIT_XOR, left, parseBitXor(p));
  }

  return left;
}

// bit_and ::= equality | equality '&' bit_and
ASTNode *parseBitAnd(Parser *p) {
  ASTNode *left = parseEquality(p);
  if (parserPeek(p) == BIT_AND) {
    parserConsume(p);
    return astBinOp(BIT_AND, left, parseBitAnd(p));
  }

  return left;
}

// equality ::= shift | shift '==' equality | shift '!=' equality
ASTNode *parseEquality(Parser *p) {
  ASTNode *left = parseShift(p);
  if (parserPeek(p) == EQ || parserPeek(p) == NEQ) {
    TokenType op = parserConsume(p).type;
    return astBinOp(op, left, parseEquality(p));
  }

  return left;
}

// shift ::= additive | shift '<<' additive | shift '>>' additive
ASTNode *parseShift(Parser *p) {
  ASTNode *left = parseAdditive(p);
  while (parserPeek(p) == LSHIFT || parserPeek(p) == RSHIFT) {
    TokenType op = parserConsume(p).type;
    ASTNode *right = parseAdditive(p);
    left = astBinOp(op, left, right);
  }

  return left;
}

// additive ::= term | additive '+' term | additive '-' term
ASTNode *parseAdditive(Parser *p) {
  ASTNode *left = parseTerm(p);
  while (parserPeek(p) == PLUS || parserPeek(p) == MINUS) {
    TokenType op = parserConsume(p).type;
    ASTNode *right = parseTerm(p);
    left = astBinOp(op, left, right);
  }

  return left;
}

// term ::= unary | term '*' unary | term '/' unary | term '%' unary
ASTNode *parseTerm(Parser *p) {
  ASTNode *left = parseUnary(p);
  while (parserPeek(p) == MUL || parserPeek(p) == DIV || parserPeek(p) == MOD) {
    TokenType op = parserConsume(p).type;
    ASTNode *right = parseUnary(p);
    left = astBinOp(op, left, right);
  }

  return left;
}

// unary ::= postfix | '-' unary | '!' unary | '++' unary | '--' unary
ASTNode *parseUnary(Parser *p) {
  TokenType tp = parserPeek(p);
  if (tp == MINUS || tp == NEG || tp == INCR || tp == DECR) {
    TokenType op = parserConsume(p).type;

    ASTNode *operand = parseUnary(p);

    if ((op == INCR || op == DECR) && (operand->type != NODE_VAR)) {
      astFree(operand);
      p->had_error = 1;
      return NULL;
    }

    if (op == MINUS || op == NEG)
      return astUnary(op, operand);
    else
      return (op == INCR) ? astPreIncr(operand) : astPreDecr(operand);
  } else
    return parsePostfix(p);
}

// postfix ::= primary | primary '++' | primary '--'
ASTNode *parsePostfix(Parser *p) {
  ASTNode *primary = parsePrimary(p);
  TokenType op = parserPeek(p);

  if (op == INCR || op == DECR) {
    parserConsume(p);
    return (op == INCR) ? astPostIncr(primary) : astPostDecr(primary);
  } else
    return primary;
}

// primary ::= number | ident | '(' expr ')'
ASTNode *parsePrimary(Parser *p) {
  TokenType tp = parserPeek(p);
  if (tp == LPAREN) {
    parserConsume(p);
    ASTNode *expr = parseExpr(p);
    if (parserPeek(p) == RPAREN) {
      parserConsume(p);
      return expr;
    }
    p->had_error = 1;
    return NULL;
  } else if (tp == NUMBER) {
    return astNum(parserConsume(p).val);
  } else if (tp == IDENT) {
    return astVar(parserConsume(p).name);
  }

  p->had_error = 1;
  return NULL;
}
