#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <iostream>
#include <map>
#include <string>
#include "ast.h"
#include "emitter.h"

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
extern std::map<std::string, llvm::AllocaInst *> NamedValues;
extern std::unique_ptr<llvm::legacy::FunctionPassManager> FPM;

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

VariableExprAST::VariableExprAST(std::string name) {
  this->name = name;
}

llvm::Value *VariableExprAST::emitllvm() {
  llvm::Value *v = NamedValues[this->name];
  if (!v) {
    std::cerr << "could not access variable " << this->name << std::endl;
    return nullptr;
  }
  return v;
}

FuncDefAST::FuncDefAST(std::string name, std::vector<std::string> param_names, std::shared_ptr<ExprAST> ret) {
  this->name = name;
  this->param_names = param_names;
  this->children.push_back(ret);
}

llvm::Value *FuncDefAST::emitllvm() {
  // set up function proto
  // set argument names
  // set up basic block
  // do stack allocation for arguments
  // emit return value

  // function proto
  std::vector<llvm::Type *>Doubles(param_names.size(), llvm::Type::getDoubleTy(context));
  llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(context), Doubles, false);
  llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, name, mymodule.get());

  // set arg names
  int idx = 0;
  for (auto &Arg : F->args()) {
    Arg.setName(this->param_names.at(idx));
    idx++;
  }

  // set up basic block
  llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "entry", F);
  builder->SetInsertPoint(BB);

  // stack allocation for arguments
  NamedValues.clear();
  for (auto &Arg : F->args()) {
      llvm::IRBuilder<> tmp_builder(&F->getEntryBlock(), F->getEntryBlock().begin());
      llvm::AllocaInst *alloca = tmp_builder.CreateAlloca(llvm::Type::getDoubleTy(context), nullptr, Arg.getName());
      builder->CreateStore(alloca, &Arg);
      NamedValues[std::string(Arg.getName())] = alloca;
  }

  // create ret
  builder->CreateRet(this->children.at(0)->emitllvm());

  // post process
  llvm::verifyFunction(*F);
  FPM->run(*F);

  // remove insert pointer
  builder->ClearInsertionPoint();

  return F;
}