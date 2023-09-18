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
 * Helper classes for the parser
*/
struct ParamList {
  std::vector<std::string> params;
};

struct ArgList {
  std::vector<int> args;
};

/**
 * Base class for all ASTs
*/
class AST {
public:
  Value value;
  std::vector<AST *> children;
  virtual int emit(Environment&) = 0;
};

/**
 * Base class for expression ASTs
*/
class ExpressionAST : public AST {};

class BinOpAST : public ExpressionAST {
public:
  BinOpAST(Value, ExpressionAST *, ExpressionAST *);
  int emit(Environment&);
  BinOpAST *getl();
  BinOpAST *getr();
};

class AssignmentAST : public AST {
public:
  AssignmentAST(Value, ExpressionAST *);
  int getrval(Environment&);
  int emit(Environment&);
};

class FuncDefAST : public AST {
public:
  FuncDefAST(Value, ParamList*, ExpressionAST *);
  ParamList *param_list;
  int emit(Environment&);
};

class FuncCallAST : public ExpressionAST {
public:
  FuncCallAST(Value, ArgList *);
  ArgList *arglist;
  int emit(Environment&);
};

/**
 * Misc
*/
/* delete and free an AST */
// XXX hack
void treefree(BinOpAST *);