/*
 * Declarations for a calculator fb3-1
 */

/* interface to the lexer */
extern int yylineno; /* from lexer */
void yyerror(const char *, ...);

/* nodes in the abstract syntax tree */
struct ast {
  int value;
  ast *l;
  ast *r;

  ast(int ,ast *, ast*);
  int eval();
};

/* evaluate an AST */
int eval(ast *);

/* delete and free an AST */
// XXX hack
void treefree(ast *);
