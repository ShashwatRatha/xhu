#ifndef ERRORS_H_
#define ERRORS_H_

#include <stdio.h>
#include <unistd.h>

typedef enum : unsigned char {
  eNullNode,
  eDivByZero,
  eUnknownOpr,
  eUnknownNodeType,
  eUndefVar,
  eMemAlloc,
  eUndefFunc,
  eMismatchFuncCall
} ErrCodes;

static inline void printError(unsigned char errCode) {
  static const char *const msgs[] = {
    [eNullNode] = "internal error: null AST node encountered",
    [eDivByZero] = "arithmetic error: division by zero",
    [eUnknownOpr] = "internal error: unrecognised operator in AST",
    [eUnknownNodeType] = "internal error: unrecognised AST node type",
    [eUndefVar] = "name error: variable is not defined",
    [eMemAlloc] = "internal error: memory allocation failed",
    [eUndefFunc] = "name error: function is not defined",
    [eMismatchFuncCall] =
        "type error: wrong number of arguments in function call",
  };

  if (errCode < sizeof msgs / sizeof *msgs && msgs[errCode])
    dprintf(STDERR_FILENO, "alg: %s\n", msgs[errCode]);
  else
    dprintf(STDERR_FILENO, "alg: unknown error (code %u)\n", (unsigned)errCode);
}

#endif
