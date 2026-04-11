#ifndef SYMTABLE_H_
#define SYMTABLE_H_

#include <stddef.h>

/* Maximum number of variables per session. */
#define SYM_CAPACITY 64

/* Return codes for sym* operations. */
typedef enum {
  SYM_OK,        /* operation succeeded                          */
  SYM_DIVZERO,   /* division by zero error                       */
  SYM_ERR_FULL,  /* table is at capacity                         */
  SYM_ERR_UNDEF, /* variable is not bound                        */
  SYM_ERR_NAME   /* reserved for future name-length validation   */
} SymResult;

/* One name→value binding. name is heap-allocated (strdup). */
typedef struct {
  char *name;
  int val;
} Symbol;

/* Flat array symbol table — linear scan, sufficient for interactive use. */
typedef struct {
  Symbol entries[SYM_CAPACITY];
  int count;
} SymTable;

/* Zero-initialise a SymTable before first use. */
void symInit(SymTable *t);

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
