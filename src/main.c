#include "eval.h"
#include "lexer.h"
#include "parser.h"
#include "symtable.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERSION "1.0"
#define PROMPT ">>> "

// runLine — lex, parse, and evaluate one line of source.
static void runLine(const char *line, SymTable *sym) {
  if (!line || line[0] == '\0')
    return;

  char *buf = strdup(line);
  if (!buf) {
    perror("strdup");
    return;
  }

  TokenArr toks = emitToks(buf);
  if (!toks.tokens) {
    free(buf);
    return;
  }

  Parser p;
  parserInit(&p, toks, sym);
  ASTNode *tree = parseProgram(&p);

  if (!p.had_error && tree)
    evalAST(tree, sym);

  astFree(tree);
  freeTokens(toks.tokens);
  free(buf);
}

// runFile — execute every line of a source file.
static void runFile(const char *path, SymTable *sym) {
  FILE *f = fopen(path, "r");
  if (!f) {
    fprintf(stderr, "alg: cannot open '%s': %s\n", path, strerror(errno));
    return;
  }

  char *line = NULL;
  size_t cap = 0;
  ssize_t len;

  while ((len = getline(&line, &cap, f)) != -1) {
    if (len > 0 && line[len - 1] == '\n')
      line[len - 1] = '\0';
    runLine(line, sym);
  }

  free(line);
  fclose(f);
}

// runREPL — interactive read-eval-print loop.
static void runREPL(SymTable *sym) {
  printf("alg %s  —  integer algebra interpreter\n", VERSION);
  printf("  arithmetic : + - * / %%\n");
  printf("  bitwise    : & | ^ << >>\n");
  printf("  logical    : && || !\n");
  printf("  equality   : == !=\n");
  printf("  incr/decr  : ++x  x++  --x  x--\n");
  printf("  assignment : x = expr   (compound: += -= *= /= <<= >>=)\n");
  printf("  'show'  prints all current variable bindings\n");
  printf("  Ctrl-D  to exit\n\n");

  char *line = NULL;
  size_t cap = 0;
  ssize_t len;

  while (1) {
    printf(PROMPT);
    fflush(stdout);

    len = getline(&line, &cap, stdin);
    if (len == -1) { // EOF — Ctrl-D
      printf("\n");
      break;
    }
    if (len > 0 && line[len - 1] == '\n')
      line[len - 1] = '\0';

    runLine(line, sym);
  }

  free(line);
}

int main(int argc, char *argv[]) {
  SymTable sym;
  symInit(&sym);

  if (argc > 1)
    runFile(argv[1], &sym);
  else
    runREPL(&sym);

  symFree(&sym);
  return 0;
}
