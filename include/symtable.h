#ifndef SYMTABLE_H_
#define SYMTABLE_H_

#include <stddef.h>

#define SYM_INIT 32

/* Return codes for sym* operations. */
typedef enum {
  SYM_OK,        /* operation succeeded                          */
  SYM_DIVZERO,   /* division by zero error                       */
  SYM_MEM_ALLOC, /* table is at capacity                         */
  SYM_ERR_UNDEF, /* variable is not bound                        */
  SYM_ERR_NAME   /* reserved for future name-length validation   */
} SymResult;

/* One name→value binding. name is heap-allocated (strdup). */
typedef struct
{
  char *name;
  int val;
} Symbol;

typedef struct SymTable
{
  Symbol *entries;
  struct SymTable *parent;
  size_t count;
  size_t capacity;
} SymTable;

/* Zero-initialise a SymTable before first use. */
SymTable *symInit(SymTable *parent);

/* Create or overwrite a binding.
 * op == ASSGN  — create if absent, overwrite if present.
 * op == other  — compound assignment; fails with SYM_ERR_UNDEF if absent. */
SymResult symSet(SymTable *t, const char *name, int val, int op);

/* Retrieve the value bound to name into *out. */
SymResult symGet(SymTable *t, const char *name, int *out);

/* Print all current bindings to stdout. */
void symShow(const SymTable *t);

/* Free all heap-allocated names and reset count. */
void symFree(SymTable *t);

#endif /* SYMTABLE_H_ */
