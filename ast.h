#include <vector>
#include <map>
#include <string>
// LLVM
#include "llvm/IR/Value.h"

// Forward decl
class BinOpAST;

/* interface to the lexer */
extern int yylineno; /* from lexer */
void yyerror(const char *, ...);

enum class UValueType { String, Integer };

struct UValue {
    UValueType vt;
    union {
        char *s;
        int d;
    };
};

UValue make_integer_value(int);
UValue make_string_value(char *);

/**
 * Helper classes for the parser
*/
struct ParamList {
  std::vector<std::string> params;
};

struct ArgList {
  std::vector<BinOpAST *> args;
};

/**
 * Base class for all ASTs
*/
class AST {
public:
  UValue value;
  std::vector<AST *> children;
};

class BinOpAST : public AST {
public:
  BinOpAST(int, BinOpAST *, BinOpAST *);
  BinOpAST(char *, BinOpAST *, BinOpAST *);
  BinOpAST *getl();
  BinOpAST *getr();
  llvm::Value *emitllvm();
};

class AssignmentAST : public AST {
public:
  AssignmentAST(char *, BinOpAST *);
  int getrval();
  llvm::Value *emitllvm();
};

class FuncDefAST : public AST {
public:
  FuncDefAST(char *, ParamList*, BinOpAST *);
  ParamList *param_list;
  llvm::Function *emitllvm();
};

class FuncCallAST : public AST {
public:
  FuncCallAST(char *, ArgList*);
  ArgList *args;
  llvm::Value *emitllvm();
};


/**
 * Misc
*/
/* delete and free an AST */
// XXX hack
void treefree(BinOpAST *);