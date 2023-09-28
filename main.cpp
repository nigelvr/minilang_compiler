#include <cstdlib>
#include <cstring>
#include <iostream>
#include <fstream>
#include <map>
// driver
#include "minidriver.hpp"
// LLVM library
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Function.h"
// function pass
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"

llvm::LLVMContext context;
std::unique_ptr<llvm::Module> mymodule;
std::unique_ptr<llvm::IRBuilder<>> builder;
std::unique_ptr<llvm::legacy::FunctionPassManager> FPM;

std::map<std::string, std::shared_ptr<llvm::Value>> NamedValues;

void init_llvm_objects() {
   mymodule = std::make_unique<llvm::Module>("test", context);
   builder = std::make_unique<llvm::IRBuilder<>>(context);
   FPM = std::make_unique<llvm::legacy::FunctionPassManager>(mymodule.get());

   FPM->add(llvm::createInstructionCombiningPass()); // peephole
   FPM->add(llvm::createReassociatePass());
   FPM->add(llvm::createGVNPass()); // eliminate common subexpressions
   FPM->add(llvm::createCFGSimplificationPass()); // simplify cfg
   FPM->doInitialization();
}

int main(const int argc, const char **argv) {
   init_llvm_objects();

   std::filebuf fb;
   fb.open(std::string(argv[1]), std::ios::in);
   std::istream is(&fb);

   MiniCompiler::Driver driver(is);
   
   int parse_result = driver.parse();
   if (parse_result) {
      std::cout << "Failed to parse file " << std::string(argv[1]) << std::endl;
      return parse_result;
   }

   // create function with expression
   std::vector<llvm::Type *>Doubles(0, llvm::Type::getDoubleTy(context));
   llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(context), Doubles, false);
   llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "foo", mymodule.get());
   llvm::BasicBlock *BB = llvm::BasicBlock::Create(context, "entry", F);
   builder->SetInsertPoint(BB);

   mymodule->print(llvm::errs(), nullptr);

   return 0;
}
