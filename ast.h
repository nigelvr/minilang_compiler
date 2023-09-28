#ifndef AST_H
#define AST_H

#include <variant>
#include <vector>
#include <map>
#include <string>
// LLVM
#include "llvm/IR/Value.h"

// Forward decl
class BinOpAST;
class ExprAST;

/**
 * Base class for all ASTs
*/
class AST {
public:
   virtual ~AST();
   std::variant<int, std::string> value;
   std::vector<std::shared_ptr<AST>> children;

   virtual llvm::Value *emitllvm() = 0;
};

class ExprAST : public AST {
public:
   virtual ~ExprAST();
   // virtual llvm::Value *emitllvm();
};

class BinOpAST : public ExprAST {
public:
   BinOpAST(std::variant<int, std::string>, std::shared_ptr<ExprAST>, std::shared_ptr<ExprAST>);
   std::shared_ptr<ExprAST> getl();
   std::shared_ptr<ExprAST> getr();
   llvm::Value *emitllvm() override;
};

#endif
