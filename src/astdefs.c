#include "lexer.h"
#include "parser.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// default constructor for AST node
static ASTNode *astInit(void) {
  ASTNode *node = calloc(1, sizeof(ASTNode));
  return node;
}

// number node
ASTNode *astNum(int val) {
  ASTNode *node = astInit();

  *node = (ASTNode){ .type = NODE_NUM, .val = val };

  return node;
}

// variable node
ASTNode *astVar(const char *name) {
  ASTNode *node = astInit();

  *node = (ASTNode){ .type = NODE_VAR, .name = strdup(name) };

  return node;
}

// binary operation node
ASTNode *astBinOp(TokenType tp, ASTNode *left, ASTNode *right) {
  ASTNode *node = astInit();

  *node = (ASTNode){ .type = NODE_BINOP,
                     .binOp = { .op = tp, .left = left, .right = right } };

  return node;
}

// unary operation node
ASTNode *astUnary(TokenType op, ASTNode *operand) {
  ASTNode *node = astInit();

  *node = (ASTNode){ .type = NODE_UNARY,
                     .standaloneNode = { .op = op, .operand = operand } };

  return node;
}

// pre-increment opearator node
ASTNode *astPreIncr(ASTNode *operand) {
  ASTNode *node = astInit();

  *node = (ASTNode){ .type = NODE_PRE_INC,
                     .standaloneNode = { .operand = operand } };

  return node;
}

// pre-decrement operator node
ASTNode *astPreDecr(ASTNode *operand) {
  ASTNode *node = astInit();

  *node = (ASTNode){ .type = NODE_PRE_DEC,
                     .standaloneNode = { .operand = operand } };

  return node;
}

// post-increment operator node
ASTNode *astPostIncr(ASTNode *operand) {
  ASTNode *node = astInit();

  *node = (ASTNode){ .type = NODE_POST_INC,
                     .standaloneNode = { .operand = operand } };

  return node;
}

// post-decrement operator node
ASTNode *astPostDecr(ASTNode *operand) {
  ASTNode *node = astInit();

  *node = (ASTNode){ .type = NODE_POST_DEC,
                     .standaloneNode = { .operand = operand } };

  return node;
}

// assignment statement node
ASTNode *astAssgn(TokenType op, const char *name, ASTNode *operand) {
  ASTNode *node = astInit();

  *node = (ASTNode){ .type = NODE_ASSIGN,
                     .assgnNode = {
                         .op = op, .name = strdup(name), .rvalue = operand } };

  return node;
}

// node for show keyword
ASTNode *astShow(void) {
  ASTNode *node = astInit();

  *node = (ASTNode){ .type = NODE_SHOW };

  return node;
}

// expression node
ASTNode *astExprStmt(ASTNode *expr) {
  ASTNode *node = astInit();

  *node = (ASTNode){ .type = NODE_EXPR_STMT,
                     .standaloneNode = { .operand = expr } };

  return node;
}

// complete program node
ASTNode *astStmts(ASTNode **stmts, size_t count) {
  ASTNode *node = astInit();

  *node = (ASTNode){ .type = NODE_PROGRAM,
                     .block = { .stmts = stmts, .stmtCount = count } };

  return node;
}

ASTNode *astReturn(ASTNode *expr) {
  ASTNode *node = astInit();

  *node =
      (ASTNode){ .type = NODE_RETURN, .standaloneNode = { .operand = expr } };

  return node;
}

ASTNode *astBreak() {
  ASTNode *node = astInit();

  *node = (ASTNode){ .type = NODE_BREAK };

  return node;
}

ASTNode *astContinue() {
  ASTNode *node = astInit();

  *node = (ASTNode){ .type = NODE_CONTINUE };

  return node;
}

ASTNode *astIf(ASTNode *condition, ASTNode *thenBlock, ASTNode *elseBlock) {
  ASTNode *node = astInit();

  *node = (ASTNode){ .type = NODE_IF,
                     .ifNode = { .condition = condition,
                                 .thenBlock = thenBlock,
                                 .elseBlock = elseBlock } };

  return node;
}

ASTNode *astElse(ASTNode *elseBlock) {
  ASTNode *node = astInit();

  *node = (ASTNode){ .type = NODE_ELSE,
                     .standaloneNode = { .operand = elseBlock } };

  return node;
}

ASTNode *astPrint(ASTNode *expression) {
  ASTNode *node = astInit();

  *node = (ASTNode){ .type = NODE_PRINT,
                     .standaloneNode = { .operand = expression } };

  return node;
}

/*
ASTNode* astFor(ASTNode* init, ASTNode* cond, ASTNode* update) {
  ASTNode* node = astInit();

  node->type = NODE_FOR;
  // for i:[0,121] {}
  // "for" IDENT ":" "[" <start> "," <end> "]" <block>
}*/

ASTNode *astWhile(ASTNode *condition, ASTNode *body) {
  ASTNode *node = astInit();

  *node = (ASTNode){ .type = NODE_WHILE,
                     .whileNode = { .condition = condition, .body = body } };

  return node;
}

void astFree(ASTNode *node) {
  if (!node)
    return;

  switch (node->type) {
  case NODE_VAR:
    free(node->name);
    break;
  case NODE_IF:
    astFree(node->ifNode.condition);
    astFree(node->ifNode.thenBlock);
    astFree(node->ifNode.elseBlock);
    break;
  case NODE_WHILE:
    astFree(node->whileNode.condition);
    astFree(node->whileNode.body);
    break;
  case NODE_BINOP:
    astFree(node->binOp.left);
    astFree(node->binOp.right);
    break;
  case NODE_ASSIGN:
    free(node->assgnNode.name);
    astFree(node->assgnNode.rvalue);
    break;
  case NODE_PROGRAM:
    for (size_t i = 0; i < node->block.stmtCount; i++) {
      astFree(node->block.stmts[i]);
    }
    free(node->block.stmts);
    break;
  case NODE_EXPR_STMT:
  case NODE_UNARY:
  case NODE_PRE_INC:
  case NODE_PRE_DEC:
  case NODE_POST_INC:
  case NODE_POST_DEC:
    astFree(node->standaloneNode.operand);
    break;
  default:
    break;
  }

  free(node);
}
