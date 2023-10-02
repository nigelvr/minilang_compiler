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
  this->l = l;
  this->r = r;
}

llvm::Value *BinOpAST::emitllvm()
{
  if (this->l && this->r) {
    llvm::Value *lv = this->l->emitllvm();
    llvm::Value *rv = this->r->emitllvm();
    switch(std::get<int>(this->value)) {
      case '+':
        return builder->CreateFAdd(lv, rv, "addtmp");
      case '-':
        return builder->CreateFSub(lv, rv, "subtmp");
      case '*':
        return builder->CreateFMul(lv, rv, "multmp");
      case '/':
        return builder->CreateFDiv(lv, rv, "divtmp");
      case '<':
        llvm::Value *temp = builder->CreateFCmpULT(lv, rv, "cmptmp");
        return builder->CreateUIToFP(temp, llvm::Type::getDoubleTy(context), "cmttmp_double");
    }
  }
  return llvm::ConstantFP::get(context, llvm::APFloat((double)std::get<int>(this->value)));
}

/**
 * Variables
*/
VariableExprAST::VariableExprAST(std::string name) {
  this->name = name;
}

llvm::Value *VariableExprAST::emitllvm() {
  llvm::AllocaInst *v = NamedValues[this->name];
  if (!v) {
    std::cerr << "could not access variable " << this->name << std::endl;
    return nullptr;
  }
  // return v;
  return builder->CreateLoad(v->getAllocatedType(), v, this->name.c_str());
}

/**
 * Statements
 * 
 * ReturnAST
 * BranchAST
*/

/**
 * Return statement
*/
ReturnAST::ReturnAST(std::shared_ptr<ExprAST> expr) {
  this->expr = expr;
}

llvm::Value *ReturnAST::emitllvm() {
  return this->expr->emitllvm();
}

/**
 * Branch, i.e. If statement
*/
BranchAST::BranchAST(std::shared_ptr<ExprAST> cond, std::shared_ptr<StatementAST> ifconseq, std::shared_ptr<StatementAST> elseconseq) {
  this->cond = cond;
  this->ifconseq = ifconseq;
  this->elseconseq = elseconseq;
}

llvm::Value *BranchAST::emitllvm() {
  llvm::Value *condv = this->cond->emitllvm();

  // convert bool to float
  condv = builder->CreateFCmpONE(condv, llvm::ConstantFP::get(context, llvm::APFloat(0.0)), "ifcond");

  llvm::Function *F = builder->GetInsertBlock()->getParent();

  // set up basic blocks
  llvm::BasicBlock *ifconseq_block = llvm::BasicBlock::Create(context, "ifconseq", F);
  llvm::BasicBlock *elseconseq_block = llvm::BasicBlock::Create(context, "elseconseq");
  llvm::BasicBlock *merge_block = llvm::BasicBlock::Create(context, "merge");

  // cond branch instruction
  builder->CreateCondBr(condv, ifconseq_block, elseconseq_block);

  // ifconseq
  builder->SetInsertPoint(ifconseq_block);
  llvm::Value *ifconseq_v = this->ifconseq->emitllvm();
  if (!ifconseq_v) {
    return nullptr;
  }
  builder->CreateBr(merge_block);

  // relaign ifconseq block
  ifconseq_block = builder->GetInsertBlock();

  // elseconseq
  F->insert(F->end(), elseconseq_block);
  builder->SetInsertPoint(elseconseq_block);
  llvm::Value *elseconseq_v = this->elseconseq->emitllvm();
  if (!elseconseq_v) {
    return nullptr;
  }
  builder->CreateBr(merge_block);

  // realign elseconseq block
  elseconseq_block = builder->GetInsertBlock();

  // merge block
  F->insert(F->end(), merge_block);
  builder->SetInsertPoint(merge_block);
  llvm::PHINode *pn = builder->CreatePHI(llvm::Type::getDoubleTy(context), 2, "phitmp");

  pn->addIncoming(ifconseq_v, ifconseq_block);
  pn->addIncoming(elseconseq_v, elseconseq_block);
  return pn;
}

FuncDefAST::FuncDefAST(std::string name, std::vector<std::string> param_names, std::shared_ptr<StatementAST> statement) {
  this->name = name;
  this->param_names = param_names;
  this->statement = statement;
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
      builder->CreateStore(&Arg, alloca);
      NamedValues[std::string(Arg.getName())] = alloca;
  }

  // create ret
  builder->CreateRet(this->statement->emitllvm());

  // post process
  llvm::verifyFunction(*F);
  FPM->run(*F);

  // remove insert pointer
  builder->ClearInsertionPoint();

  return F;
}