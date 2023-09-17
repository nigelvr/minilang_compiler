#include <vector>
#include <map>
#include <string>
#include "env.h"

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

/**
 * Base class for all ASTs
*/
class AST {
public:
  Value value;
  std::vector<AST *> children;
  virtual int eval(Environment&) = 0;
};

/**
 * Base class for expression ASTs
*/
class ExpressionAST : AST {

}

class BinOpAST : public AST {
public:
  BinOpAST(Value, BinOpAST *, BinOpAST *);
  int eval(Environment&);
  BinOpAST *getl();
  BinOpAST *getr();
};

class AssignmentAST : public AST {
public:
  AssignmentAST(Value, BinOpAST *);
  int getrval(Environment&);
  int eval(Environment&);
};

class FuncDefAST : public AST {
public:
  FuncDefAST(Value, std::vector<std::string>, BinOpAST *);
  std::vector<std::string> param_list;
  int eval(Environment&);
};

class FuncCallAST : public AST {
public:
  FuncCallAST(Value, std::vector<int>);
  std::vector<int> arg_list;
  int eval(Environment&);
};

/**
 * Misc
*/
/* delete and free an AST */
// XXX hack
void treefree(BinOpAST *);