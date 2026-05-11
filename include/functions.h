#ifndef FUNC_H
#define FUNC_H

#include <stddef.h>

struct ASTNode;
typedef struct ASTNode ASTNode;

typedef struct
{
  char *name;
  ASTNode *paramList;
  ASTNode *funcBody;
} Function;

typedef struct
{
  Function *funcs;
  size_t funcCount;
  size_t tableCap;
} FuncTable;

FuncTable *funcTableInit();
char funcTableAdd(FuncTable *fTable, char *name, ASTNode *paramList,
                  ASTNode *funcBody);
long int getFuncFromTable(const FuncTable *fTable, const char *funcName);
void freeFuncTable(FuncTable *fTable);

#endif
