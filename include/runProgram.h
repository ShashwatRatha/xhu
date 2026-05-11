#ifndef RUNPROGRAM_H
#define RUNPROGRAM_H

#include "functions.h"
#include "symtable.h"

void runLine(const char *line, SymTable *sym, FuncTable *funcs);
void runFile(const char *path, SymTable *sym, FuncTable *funcs);
void runREPL(SymTable *sym, FuncTable *ftable);

#endif
