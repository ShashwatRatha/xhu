#include "eval.h"
#include "lexer.h"
#include "parser.h"
#include "symtable.h"
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

typedef enum : unsigned char {
  eNullNode,
  eDivByZero,
  eUnknownOpr,
  eUnknownNodeType,
  eUndefVar
} ErrCodes;

typedef enum : unsigned char { sProgExecution } SuccessCodes;

Result evalAST(ASTNode *node, SymTable *table) {
  if (!node) {
    return failure(eNullNode);
  }

  switch (node->type) {
  case nodeBreak:
    return brek();
  case nodeContinue:
    return cnt();
  case nodeReturn: {
    Result retVal = evalAST(node->standaloneNode.operand, table);
    if (retVal.status == ERR)
      return retVal;

    if (retVal.status == VAL)
      return ret(retVal.returnVal);
    return ret(0);
  }
  case nodeFor: {
    SymTable *inTable = symInit(table);
    Result init = evalAST(node->forNode.init, inTable);
    if (init.status == ERR)
      return init;

    for (;;) {
      Result cond = evalAST(node->forNode.cond, inTable);
      if (cond.status == ERR) {
        symFree(inTable);
        return cond;
      }
      if (cond.returnVal == 0)
        break;

      Result body = evalAST(node->forNode.body, inTable);
      if (body.status == ERR || body.status == RET) {
        symFree(inTable);
        return body;
      }
      if (body.status == BRK)
        break;

      Result update = evalAST(node->forNode.update, inTable);
      if (update.status == ERR) {
        symFree(inTable);
        return update;
      }
    }

    symFree(inTable);
    return success(sProgExecution);
  }
  case nodeWhile: {
    SymTable *inTable = symInit(table);
    while (1) {
      Result condition = evalAST(node->whileNode.condition, inTable);
      if (condition.status == ERR) {
        symFree(inTable);
        return condition;
      }
      if (condition.returnVal == 0)
        break;

      Result body = evalAST(node->whileNode.body, inTable);
      if (body.status == ERR || body.status == RET) {
        symFree(inTable);
        return body;
      }
      if (body.status == BRK)
        break;
    }

    symFree(inTable);
    return success(sProgExecution);
  }
    // case nodeFn:
    // case nodeFnCall:
    //                 break;
  case nodeProgram:
    return evalAST(node->standaloneNode.operand, table);
  case nodeBlock: {
    SymTable *inTable = symInit(table);
    Result res = evalAST(node->standaloneNode.operand, inTable);
    symFree(inTable);
    return res;
  }
  case nodeVar: {
    int val = 0;
    if (symGet(table, node->name, &val) != SYM_OK)
      return failure(eUndefVar);
    return value(val);
  }
  case nodeNum:
    return value(node->val);
  case nodeShow:
    // symShow(table);
    return success(sProgExecution);
  case nodePrint: {
    Result exprVal = evalAST(node->standaloneNode.operand, table);
    if (exprVal.status != VAL)
      return exprVal;

    printf("%d\n", exprVal.returnVal);
    return success(sProgExecution);
  }
  case nodeExprStmt:
    return evalAST(node->standaloneNode.operand, table);
  case nodeStmts:
    for (size_t i = 0; i < node->block.stmtCount; i++) {
      Result res = evalAST(node->block.stmts[i], table);
      if (res.status != VAL && res.status != SUC)
        return res;
    }
    return success(sProgExecution);
  case nodeIf: {
    Result condition = evalAST(node->ifNode.condition, table);
    if (condition.status == ERR)
      return condition;
    if (condition.status != VAL)
      return failure(eUnknownNodeType);
    if (condition.returnVal != 0)
      return evalAST(node->ifNode.thenBlock, table);
    else if (node->ifNode.elseBlock)
      return evalAST(node->ifNode.elseBlock, table);

    return success(sProgExecution);
  }
  case nodePreInc: {
    const char *name = node->name;
    int val = 0;
    if (symGet(table, name, &val) != SYM_OK)
      return failure(eUndefVar);
    symSet(table, name, val + 1, ASSGN);
    return value(val + 1);
  }
  case nodePreDec: {
    const char *name = node->name;
    int val = 0;
    if (symGet(table, name, &val) != SYM_OK)
      return failure(eUndefVar);
    symSet(table, name, val - 1, ASSGN);
    return value(val - 1);
  }
  case nodePostInc: {
    const char *name = node->name;
    int val = 0;
    if (symGet(table, name, &val) != SYM_OK)
      return failure(eUndefVar);
    symSet(table, name, val + 1, ASSGN);
    return value(val);
  }
  case nodePostDec: {
    const char *name = node->name;
    int val = 0;
    if (symGet(table, name, &val) != SYM_OK)
      return failure(eUndefVar);
    symSet(table, name, val - 1, ASSGN);
    return value(val);
  }
  case nodeAssign: {
    Result rval = evalAST(node->assgnNode.rvalue, table);
    if (rval.status == ERR)
      return rval;

    SymResult res =
        symSet(table, node->assgnNode.name, rval.returnVal, node->assgnNode.op);

    if (res == SYM_ERR_UNDEF)
      return failure(eUndefVar);
    else if (res == SYM_DIVZERO)
      return failure(eDivByZero);
    else
      return value(rval.returnVal);
    // return the value that was actually stored
    // to preserve semantic meaning
    // int newval = 0;
    // symGet(table, node->name, &newval);
    // return newval;
  }
  case nodeBinOp: {
    Result left = evalAST(node->binOp.left, table);
    if (left.status == ERR)
      return left;

    Result right = evalAST(node->binOp.right, table);
    if (right.status == ERR)
      return right;

    switch (node->binOp.op) {
    case PLUS:
      return value(left.returnVal + right.returnVal);
    case MINUS:
      return value(left.returnVal - right.returnVal);
    case MUL:
      return value(left.returnVal * right.returnVal);
    case DIV:
      if (right.returnVal == 0)
        return failure(eDivByZero);
      return value(left.returnVal / right.returnVal);
    case MOD:
      if (right.returnVal == 0)
        return failure(eDivByZero);
      return value(left.returnVal % right.returnVal);
    case AND:
      return value(left.returnVal && right.returnVal);
    case OR:
      return value(left.returnVal || right.returnVal);
    case BIT_AND:
      return value(left.returnVal & right.returnVal);
    case BIT_OR:
      return value(left.returnVal | right.returnVal);
    case BIT_XOR:
      return value(left.returnVal ^ right.returnVal);
    case LSHIFT:
      return value(left.returnVal << right.returnVal);
    case RSHIFT:
      return value(left.returnVal >> right.returnVal);
    case EQ:
      return value(left.returnVal == right.returnVal);
    case NEQ:
      return value(left.returnVal != right.returnVal);
    case LT:
      return value(left.returnVal < right.returnVal);
    case LET:
      return value(left.returnVal <= right.returnVal);
    case GT:
      return value(left.returnVal > right.returnVal);
    case GET:
      return value(left.returnVal >= right.returnVal);
    default:
      return failure(eUnknownOpr);
    }
  }
  case nodeUnary: {
    Result opr = evalAST(node->standaloneNode.operand, table);
    if (opr.status == ERR)
      return opr;

    switch (node->standaloneNode.op) {
    case NOT:
      return value(!opr.returnVal);
    case BIT_NOT:
      return value(~opr.returnVal);
    case MINUS:
      return value(-opr.returnVal);
    default:
      return failure(eUnknownOpr);
    }
  }
  default:
    return failure(eUnknownNodeType);
  }
}
