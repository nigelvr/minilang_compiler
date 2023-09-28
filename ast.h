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
   virtual ~AST() = 0;
   virtual llvm::Value *emitllvm() = 0;
   std::variant<int, std::string> value;
   std::vector<std::shared_ptr<AST>> children;
};

class ExprAST : public AST {};

class BinOpAST : public ExprAST {
public:
   BinOpAST(std::variant<int, std::string>, std::shared_ptr<ExprAST>, std::shared_ptr<ExprAST>);
   std::shared_ptr<ExprAST> getl();
   std::shared_ptr<ExprAST> getr();
   llvm::Value *emitllvm() override;
};

#endif
