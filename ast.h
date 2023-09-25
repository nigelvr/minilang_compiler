#include <vector>
#include <map>
#include <string>
// LLVM
#include "llvm/IR/Value.h"

// Forward decl
class BinOpAST;
class ExprAST;
class FuncPart;

/* interface to the lexer */
extern int yylineno; /* from lexer */
void yyerror(const char *, ...);

enum class UValueType { String, Integer };
enum class FuncPartType { Return, If };

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
  std::vector<ExprAST *> args;
};

struct FuncPartList {
  std::vector<FuncPart *> fparts;
};

/**
 * Base class for all ASTs
 * 
 * XXX TODO: destructor for this class
*/
class AST {
public:
  UValue value;
  std::vector<AST *> children;
};

class ExprAST : public AST {
public:
  virtual llvm::Value *emitllvm() = 0;
};

class BinOpAST : public ExprAST {
public:
  BinOpAST(int, ExprAST *, ExprAST *);
  BinOpAST(char *, ExprAST *, ExprAST *);
  ExprAST *getl();
  ExprAST *getr();
  llvm::Value *emitllvm();
};

class FuncDefAST : public AST {
public:
  FuncDefAST(char *, ParamList*, FuncPart *);
  ParamList *param_list;
  FuncPart *funcpart;
  llvm::Function *emitllvm();
};

class FuncCallAST : public ExprAST {
public:
  FuncCallAST(char *, ArgList*);
  ArgList *args;
  llvm::Value *emitllvm();
};

class FuncPart {
public:
  FuncPart(FuncPartType, ExprAST *, ExprAST *, FuncPart *);
  FuncPartType fpt;
  ExprAST *return_fp;
  ExprAST *ifcond;
  FuncPart *ifconseq;
};