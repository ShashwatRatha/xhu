#include "eval.h"
#include "lexer.h"
#include "parser.h"
#include "symtable.h"
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

/* ===Some Implementation Details===
 *
 * astFree is not called in evalAST for each node as there are parent owns
 * children, so could result in double frees. Caller frees the AST nodes after
 * evaluation. 0 is usd as an error-sentinel, but needs to be fixed somehow this
 * causes some bugs (such as x = y with undefined y will set x = 0.) (maybe by
 * changing the return type) Assignment nodes are dealt with separately from
 * other binary operation nodes, as trigger manipulation of the symbol table
 * itself Increment and Decrement is handled separately from other assignment
 * operations, as they are unary operations
 */

int evalAST(ASTNode *node, SymTable *table) {
  if (!node)
    return 0;

  switch (node->type) {

  case NODE_SHOW: {
    symShow(table);
    return 0;
  }

  case NODE_NUM: {
    return node->ival;
  }

  case NODE_VAR: {
    int val = 0;
    if (symGet(table, node->name, &val) != SYM_OK)
      dprintf(STDERR_FILENO, "error: undefined variable '%s'\n", node->name);
    return val;
  }

  case NODE_BINOP: {
    int left = evalAST(node->left, table);
    int right = evalAST(node->right, table);
    switch (node->op) {
    case PLUS:
      return left + right;
    case MINUS:
      return left - right;
    case MUL:
      return left * right;
    case DIV:
      if (right == 0) {
        dprintf(STDERR_FILENO, "error: division by zero\n");
        return 0;
      }
      return left / right;
    case MOD:
      if (right == 0) {
        dprintf(STDERR_FILENO, "error: modulo by zero\n");
        return 0;
      }
      return left % right;
    case AND:
      return left && right;
    case OR:
      return left || right;
    case BIT_AND:
      return left & right;
    case BIT_OR:
      return left | right;
    case BIT_XOR:
      return left ^ right;
    case LSHIFT:
      return left << right;
    case RSHIFT:
      return left >> right;
    case EQ:
      return left == right;
    case NEQ:
      return left != right;
    default:
      return 0;
    }
  }

  case NODE_UNARY: {
    int val = evalAST(node->left, table);
    switch (node->op) {
    case MINUS:
      return -val;
    case NEG:
      return !val;
    default:
      return 0;
    }
  }

  case NODE_PRE_INC: {
    // ++x: increment then return new value
    const char *name = node->left->name;
    int val = 0;
    if (symGet(table, name, &val) != SYM_OK) {
      dprintf(STDERR_FILENO, "error: undefined variable '%s'\n", name);
      return 0;
    }
    symSet(table, name, val + 1, ASSGN);
    return val + 1;
  }

  case NODE_PRE_DEC: {
    // --x: decrement then return new value
    const char *name = node->left->name;
    int val = 0;
    if (symGet(table, name, &val) != SYM_OK) {
      dprintf(STDERR_FILENO, "error: undefined variable '%s'\n", name);
      return 0;
    }
    symSet(table, name, val - 1, ASSGN);
    return val - 1;
  }

  case NODE_POST_INC: {
    // x++: return old value then increment
    const char *name = node->left->name;
    int val = 0;
    if (symGet(table, name, &val) != SYM_OK) {
      dprintf(STDERR_FILENO, "error: undefined variable '%s'\n", name);
      return 0;
    }
    symSet(table, name, val + 1, ASSGN);
    return val;
  }

  case NODE_POST_DEC: {
    // x--: return old value then decrement
    const char *name = node->left->name;
    int val = 0;
    if (symGet(table, name, &val) != SYM_OK) {
      dprintf(STDERR_FILENO, "error: undefined variable '%s'\n", name);
      return 0;
    }
    symSet(table, name, val - 1, ASSGN);
    return val;
  }

  case NODE_ASSIGN: {
    int rval = evalAST(node->right, table);
    SymResult res = symSet(table, node->name, rval, node->op);
    if (res == SYM_ERR_UNDEF) {
      dprintf(STDERR_FILENO,
              "error: '%s' undefined (use '=' for first assignment)\n",
              node->name);
      return 0;
    }
    if (res == SYM_ERR_FULL) {
      dprintf(STDERR_FILENO, "error: symbol table full\n");
      return 0;
    }
    if (res == SYM_DIVZERO) {
      dprintf(STDERR_FILENO, "error: division by zero\n");
      return 0;
    }
    // return the value that was actually stored
    // to presserve semantic meaning
    int newval = 0;
    symGet(table, node->name, &newval);
    return newval;
  }

  case NODE_EXPR_STMT: {
    // bare expression: evaluate and print result
    int val = evalAST(node->left, table);
    printf("%d\n", val);
    return val;
  }

  case NODE_PROGRAM: {
    int val = 0;
    for (size_t i = 0; i < node->stmt_count; i++)
      val = evalAST(node->stmts[i], table);
    return val;
  }

  default:
    return 0;
  }
}
