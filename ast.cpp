#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <cstdlib>
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

/**
 * Helpers
*/
llvm::AllocaInst *make_alloca(llvm::Function *F, llvm::StringRef varname) {
  llvm::IRBuilder<> tmp_builder(&F->getEntryBlock(), F->getEntryBlock().begin());
  return tmp_builder.CreateAlloca(llvm::Type::getDoubleTy(context), nullptr, varname);
}

AST::~AST() {}

/**
 * Numbers
*/
NumberAST::NumberAST(double value) {
  this->value = value;
}

llvm::Value *NumberAST::emitllvm() {
  return llvm::ConstantFP::get(context, llvm::APFloat(this->value));
}

/**
 * Binary Operators
*/
BinOpAST::BinOpAST(BinOpType bin_op_type, std::shared_ptr<ExprAST> l, std::shared_ptr<ExprAST> r) {
  if (!l || !r) {
    std::cerr << "Error constructing BinOpAST: both l and r are null" << std::endl;
    exit(1);
  }

  this->bin_op_type = bin_op_type;
  this->l = l;
  this->r = r;
}

llvm::Value *BinOpAST::emitllvm()
{
  if (this->l && this->r) {
    llvm::Value *lv = this->l->emitllvm();
    llvm::Value *rv = this->r->emitllvm();
    llvm::Value *temp;
    switch(this->bin_op_type) {
      case BinOpType::PLUS:
        return builder->CreateFAdd(lv, rv, "addtmp");
      case BinOpType::MINUS:
        return builder->CreateFSub(lv, rv, "subtmp");
      case BinOpType::TIMES:
        return builder->CreateFMul(lv, rv, "multmp");
      case BinOpType::DIV:
        return builder->CreateFDiv(lv, rv, "divtmp");
      case BinOpType::LT:
        return builder->CreateFCmpULT(lv, rv, "cmp_lt_tmp");
        // return builder->CreateUIToFP(temp, llvm::Type::getDoubleTy(context), "cmttmp_lt_double");
      case BinOpType::EQ:
        temp = builder->CreateFCmpUEQ(lv, rv, "cmp_eq_tmp");
        return builder->CreateUIToFP(temp, llvm::Type::getDoubleTy(context), "cmttmp_eq_double");
    }
  }
  return nullptr;
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
  std::cout << "emitting CreateLoad: " << this->name << std::endl;
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
  llvm::Value *v = this->expr->emitllvm();
  builder->CreateRet(v);
  return v;
}

/**
 * Branch, i.e. If statement
*/
BranchAST::BranchAST(std::shared_ptr<ExprAST> cond, std::vector<std::shared_ptr<StatementAST>> ifconseq,
    std::vector<std::shared_ptr<StatementAST>> elseconseq) {
  this->cond = cond;
  this->ifconseq = ifconseq;
  this->elseconseq = elseconseq;
}

llvm::Value *BranchAST::emitllvm() {
  // convert bool to float
  llvm::Value *condv = this->cond->emitllvm();
  condv = builder->CreateFCmpONE(condv, llvm::ConstantFP::get(context, llvm::APFloat(0.0)), "ifcond");

  // get the function
  llvm::Function *F = builder->GetInsertBlock()->getParent();

  // set up basic blocks
  llvm::BasicBlock *ifconseq_block = llvm::BasicBlock::Create(context, "ifconseq", F);
  llvm::BasicBlock *elseconseq_block = llvm::BasicBlock::Create(context, "elseconseq", F);

  // cond branch instruction
  builder->CreateCondBr(condv, ifconseq_block, elseconseq_block);

  // ifconseq
  builder->SetInsertPoint(ifconseq_block);
  for (auto &S : this->ifconseq) {
    S->emitllvm();
  }
  builder->SetInsertPoint(elseconseq_block);

  // elseconseq
  if (this->elseconseq.size() == 0) {
    return nullptr;
  }
  for (auto &S : this->elseconseq) {
    S->emitllvm();
  }

  return nullptr;
}

/**
 * Assignment
*/
AssignmentAST::AssignmentAST(std::string ident, std::shared_ptr<ExprAST> expr) {
  this->ident = ident;
  this->expr = expr;
}

llvm::Value *AssignmentAST::emitllvm() {
  llvm::Function *F = builder->GetInsertBlock()->getParent();
  llvm::AllocaInst *alloca;
  
  if (!NamedValues.count(this->ident)) {
	alloca = make_alloca(F, this->ident);
	NamedValues[this->ident] = alloca;
  } else {
	  alloca = NamedValues[this->ident];
  }

  builder->CreateStore(this->expr->emitllvm(), alloca);
  return alloca;
}

/**
 * While loop
*/
WhileLoopAST::WhileLoopAST(std::shared_ptr<ExprAST> cond, std::vector<std::shared_ptr<StatementAST>> statements) {
  this->cond = cond;
  this->statements = statements;
}

llvm::Value *WhileLoopAST::emitllvm() {

  llvm::Function *F = builder->GetInsertBlock()->getParent();

  llvm::BasicBlock *whiletop = llvm::BasicBlock::Create(context, "whiletop", F);
  llvm::BasicBlock *whilebody = llvm::BasicBlock::Create(context, "whilebody", F);
  llvm::BasicBlock *whiledone = llvm::BasicBlock::Create(context, "whiledone", F);

  builder->CreateBr(whiletop);

  builder->SetInsertPoint(whiletop);
  llvm::Value *condv = this->cond->emitllvm();
  // condv = builder->CreateFCmpONE(condv, llvm::ConstantFP::get(context, llvm::APFloat(0.0)), "whilecond");
  builder->CreateCondBr(condv, whilebody, whiledone);
  
  builder->SetInsertPoint(whilebody);
  for (auto &S : this->statements) {
    S->emitllvm();
  }
  builder->CreateBr(whiletop);

  builder->SetInsertPoint(whiledone);

  return nullptr;
}

/**
 * Function definition
*/
FuncDefAST::FuncDefAST(std::string name, std::vector<std::string> param_names, std::vector<std::shared_ptr<StatementAST>> statements) {
  this->name = name;
  this->param_names = param_names;
  this->statements = statements;
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
      llvm::AllocaInst *alloca = make_alloca(F, Arg.getName());
      builder->CreateStore(&Arg, alloca);
      NamedValues[std::string(Arg.getName())] = alloca;
  }

  // run the return statement
  for (auto &S : this->statements) {
    S->emitllvm();
  }

  // post process
  llvm::verifyFunction(*F);
  FPM->run(*F);

  // remove insert pointer
  // builder->ClearInsertionPoint();

  return F;
}

/**
 * Function call
*/
FuncCallAST::FuncCallAST(std::string name, std::vector<std::shared_ptr<ExprAST>> param_values) {
  this->name = name;
  this->param_values = param_values;
}

llvm::Value *FuncCallAST::emitllvm() {
  llvm::Function *F = mymodule->getFunction(this->name);

  std::vector<llvm::Value *> args;
  for (auto &p : this->param_values) {
    args.push_back(p->emitllvm());
  }

  return builder->CreateCall(F, args, "calltmp");
}
