#include <vector>

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

class AST {
public:
  Value value;
  std::vector<AST *> children;
  virtual int eval() = 0;
};

class BinOpAST : public AST {
public:
  BinOpAST(Value, BinOpAST *, BinOpAST *);
  int eval();
  BinOpAST *getl();
  BinOpAST *getr();
};

class AssignmentAST : public AST {
public:
  AssignmentAST(Value, BinOpAST *);
  int getrval();
  int eval();
};

/**
 * Misc
*/
/* delete and free an AST */
// XXX hack
void treefree(BinOpAST *);