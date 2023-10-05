#ifndef AST_H
#define AST_H

#include <variant>
#include <vector>
#include <map>
#include <string>
// LLVM
#include "llvm/IR/Value.h"

// Builtin binary operator types
enum class BinOpType {
   PLUS,
   MINUS,
   TIMES,
   DIV,
   LE,
   EQ
};


// Forward decl
class BinOpAST;
class ExprAST;

/**
 * Base class for all ASTs
*/
class AST {
public:
   virtual ~AST() = 0;
   virtual llvm::Value *emitllvm() = 0;
};

class ExprAST : public AST {};

class NumberAST : public ExprAST {
public:
   NumberAST(double value);
   double value;
   llvm::Value *emitllvm() override;
};

class BinOpAST : public ExprAST {
public:
   BinOpAST(int, std::shared_ptr<ExprAST>, std::shared_ptr<ExprAST>);
   int value;
   std::shared_ptr<ExprAST> l;
   std::shared_ptr<ExprAST> r;
   llvm::Value *emitllvm() override;
};

class VariableExprAST : public ExprAST {
public:
  VariableExprAST(std::string name);
  std::string name;
  llvm::Value *emitllvm() override;
};

class StatementAST : public AST {};

class ReturnAST : public StatementAST {
public:
   ReturnAST(std::shared_ptr<ExprAST> expr);
   std::shared_ptr<ExprAST> expr;
   llvm::Value *emitllvm() override;
};

class BranchAST : public StatementAST {
public:
   BranchAST(std::shared_ptr<ExprAST> cond, std::shared_ptr<StatementAST> ifconseq, std::shared_ptr<StatementAST> elseconseq);
   std::shared_ptr<ExprAST> cond;
   std::shared_ptr<StatementAST> ifconseq;
   std::shared_ptr<StatementAST> elseconseq;
   llvm::Value *emitllvm() override;
};

class AssignmentAST : public StatementAST {
public:
   AssignmentAST(std::string ident, std::shared_ptr<ExprAST> expr);
   std::string ident;
   std::shared_ptr<ExprAST> expr;
   llvm::Value *emitllvm() override;
};

class WhileLoopAST : public StatementAST {
public:
   WhileLoopAST(std::shared_ptr<ExprAST> cond, std::vector<std::shared_ptr<StatementAST>> statements);
   std::shared_ptr<ExprAST> cond;
   std::vector<std::shared_ptr<StatementAST>> statements;
   llvm::Value *emitllvm() override;
};

class FuncDefAST : public AST {
public:
   FuncDefAST(std::string name, std::vector<std::string> param_names, std::vector<std::shared_ptr<StatementAST>> statements);
   std::string name;
   std::vector<std::string> param_names;
   std::vector<std::shared_ptr<StatementAST>> statements;
   llvm::Value *emitllvm() override;
};

class FuncCallAST : public ExprAST {
public:
   FuncCallAST(std::string name, std::vector<std::shared_ptr<ExprAST>> param_values);
   std::string name;
   std::vector<std::shared_ptr<ExprAST>> param_values;
   llvm::Value *emitllvm() override;
};



#endif
