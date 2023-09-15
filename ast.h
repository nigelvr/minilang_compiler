/*
 * Declarations for a calculator fb3-1
 */

/* interface to the lexer */
extern int yylineno; /* from lexer */
void yyerror(const char *, ...);

enum class ValueType { String, Integer };

struct Value {
    ValueType vt;
    union {
        char *s;
        int d;
    };
};

Value make_integer_value(int);
Value make_string_value(char *);

/* nodes in the abstract syntax tree */
struct ast {
  Value value;
  ast *l;
  ast *r;

  ast(Value ,ast *, ast*);
  int eval();
};

/* evaluate an AST */
int eval(ast *);

/* delete and free an AST */
// XXX hack
void treefree(ast *);
