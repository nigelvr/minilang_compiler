#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <map>
#include <string>
#include "ast.h"

// LLVM
#include "llvm/ADT/APInt.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/LegacyPassManager.h"

extern llvm::LLVMContext context;
extern std::unique_ptr<llvm::Module> mymodule;
extern std::unique_ptr<llvm::IRBuilder<>> builder;

AST::~AST() {}

/**
 * Binary Operators
*/
BinOpAST::BinOpAST(std::variant<int, std::string> value, std::shared_ptr<ExprAST> l, std::shared_ptr<ExprAST> r) {
  this->value = value;
  this->children.push_back(l);
  this->children.push_back(r);
}

std::shared_ptr<ExprAST> BinOpAST::getl() {
  if (children.size() >= 1) {
    return std::static_pointer_cast<ExprAST>(children.at(0));
  }
  return nullptr;
}

std::shared_ptr<ExprAST> BinOpAST::getr() {
  if (children.size() == 2) {
    return std::static_pointer_cast<ExprAST>(children.at(1));
  }
  return nullptr;
}

llvm::Value *BinOpAST::emitllvm()
{
  auto L = this->getl();
  auto R = this->getr();

  if (L && R) {
    llvm::Value *Lv = L->emitllvm();
    llvm::Value *Rv = R->emitllvm();
    switch(std::get<int>(this->value)) {
      case '+':
        return builder->CreateFAdd(Lv, Rv, "addtmp");
      case '-':
        return builder->CreateFSub(Lv, Rv, "subtmp");
      case '*':
        return builder->CreateFMul(Lv, Rv, "multmp");
      case '/':
        return builder->CreateFDiv(Lv, Rv, "divtmp");
      case '<':
        llvm::Value *temp = builder->CreateFCmpULT(Lv, Rv, "cmptmp");
        return builder->CreateUIToFP(temp, llvm::Type::getDoubleTy(context), "cmttmp_double");
    }
  }
  return llvm::ConstantFP::get(context, llvm::APFloat((double)std::get<int>(this->value)));
}


