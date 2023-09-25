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

extern llvm::IRBuilder<> builder;
extern llvm::LLVMContext context;
extern llvm::Module *mymodule;
extern llvm::legacy::FunctionPassManager *FPM;
extern std::map<std::string, llvm::Value *> NamedValues;

/**
 * Language underlying values
*/
UValue make_integer_value(int d) {
  UValue v;
  v.vt = UValueType::Integer;
  v.d = d;
  return v;
}

UValue make_string_value(char *s) {
  UValue v;
  v.vt = UValueType::String;
  v.s = s;
  return v;
}

/**
 * Binary Operators
*/
BinOpAST::BinOpAST(int value, ExprAST *l, ExprAST *r) {
  this->value = make_integer_value(value);
  this->children.push_back(l);
  this->children.push_back(r);
}

BinOpAST::BinOpAST(char *value, ExprAST *l, ExprAST *r) {
  this->value = make_string_value(value);
  this->children.push_back(l);
  this->children.push_back(r);
}

ExprAST *BinOpAST::getl() {
  if (children.size() >= 1) {
    return (ExprAST *)children.at(0);
  }
  return nullptr;
}

ExprAST *BinOpAST::getr() {
  if (children.size() == 2) {
    return (ExprAST *)children.at(1);
  }
  return nullptr;
}

llvm::Value *BinOpAST::emitllvm()
{
  ExprAST *L = this->getl();
  ExprAST *R = this->getr();

  if (L && R) {
    llvm::Value *Lv = L->emitllvm();
    llvm::Value *Rv = R->emitllvm();
    switch(this->value.d) {
      case '+':
        return builder.CreateFAdd(Lv, Rv, "addtmp");
      case '-':
        return builder.CreateFSub(Lv, Rv, "subtmp");
      case '*':
        return builder.CreateFMul(Lv, Rv, "multmp");
      case '/':
        return builder.CreateFDiv(Lv, Rv, "divtmp");
      case '<':
        llvm::Value *temp = builder.CreateFCmpULT(Lv, Rv, "cmptmp");
        return builder.CreateUIToFP(temp, llvm::Type::getDoubleTy(context), "cmttmp_double");
    }
  }
  if (this->value.vt == UValueType::String) {
    return NamedValues[std::string(this->value.s)];
  }
  return llvm::ConstantFP::get(context, llvm::APFloat((double)this->value.d));
}

/**
 * Function definition
 */
FuncDefAST::FuncDefAST(char *value, ParamList *param_list, FuncPart *funcpart) {
  this->value = make_string_value(value);
  this->param_list = param_list;
  this->funcpart = funcpart;
}

llvm::Function *FuncDefAST::emitllvm() {
  std::vector<llvm::Type *>Doubles(this->param_list->params.size(), llvm::Type::getDoubleTy(context));
  llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(context), Doubles, false);
  llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, this->value.s, mymodule);
  // set the param names
  int idx;
  for (auto &Arg : F->args()) {
    Arg.setName(param_list->params.at(idx++));
  }


  llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "entry", F);
  builder.SetInsertPoint(BB);

  // set the argument names
  NamedValues.clear();
  for (auto &Arg : F->args())
    NamedValues[std::string(Arg.getName())] = &Arg;

  // generate code from function part
  this->funcpart->emitllvm();

  llvm::verifyFunction(*F);

  FPM->run(*F);

  return F;
}

/**
 * Function call
*/
FuncCallAST::FuncCallAST(char *value, ArgList *a) {
  this->value = make_string_value(value);
  this->args = a;
}

llvm::Value *FuncCallAST::emitllvm() {
  llvm::Function *F = mymodule->getFunction(this->value.s);
  std::vector<llvm::Value *>evaled_args;

  for (auto &a : this->args->args) {
    evaled_args.push_back(a->emitllvm());
  }

  return builder.CreateCall(F, evaled_args, "calltmp");
}

/* Non-AST helpers */
FuncPart::FuncPart(FuncPartType fpt, ExprAST *return_fp, ExprAST *ifcond, FuncPart *ifconseq) {
  this->fpt = fpt;
  this->return_fp = return_fp;
  this->ifcond = ifcond;
  this->ifconseq = ifconseq;
}

void FuncPart::emitllvm() {
   if (this->fpt == FuncPartType::Return) {
    llvm::Value *ret = this->return_fp->emitllvm();
    builder.CreateRet(ret);
    builder.ClearInsertionPoint();
  } else {
    // if block
    llvm::Value *cond = this->ifcond->emitllvm();
    // convert cond to boolean
    cond = builder.CreateFCmpONE(cond, llvm::ConstantFP::get(context, llvm::APFloat(0.0)), "ifcond");
    
    fprintf(stderr, "Error: Only RET implemented\n");
  }
}