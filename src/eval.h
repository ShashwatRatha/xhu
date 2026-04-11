#ifndef EVAL_H_
#define EVAL_H_

#include "parser.h"
#include "symtable.h"

/* evalAST — walk the AST rooted at node and return its integer value.
 * NODE_EXPR_STMT nodes print their result to stdout automatically.
 * The caller owns the tree and must call astFree() after evaluation.
 */
int evalAST(ASTNode *node, SymTable *table);

#endif /* EVAL_H_ */
