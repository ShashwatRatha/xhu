#ifndef EVAL_H_
#define EVAL_H_

struct ASTNode;
typedef struct ASTNode ASTNode;

struct SymTable;
typedef struct SymTable SymTable;

typedef enum { SUC, ERR, VAL, BRK, CNT, RET } Status;

typedef struct
{
  Status status;
  union {
    unsigned char errCode;
    unsigned char sucCode;
    int returnVal;
  };
} Result;

static inline Result success(unsigned char sCode) {
  return (Result){ .status = SUC, .sucCode = sCode };
}
static inline Result failure(unsigned char eCode) {
  return (Result){ .status = ERR, .errCode = eCode };
}
static inline Result value(int value) {
  return (Result){ .status = VAL, .returnVal = value };
}
static inline Result brek() { return (Result){ .status = BRK }; }
static inline Result cnt() { return (Result){ .status = CNT }; }
static inline Result ret(int value) {
  return (Result){ .status = RET, .returnVal = value };
}

Result evalAST(ASTNode *node, SymTable *table);

#endif
