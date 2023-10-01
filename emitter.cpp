#include <iostream>
#include <string>
#include <map>

#include "llvm/ADT/APInt.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/LegacyPassManager.h"

extern llvm::LLVMContext context;
extern std::unique_ptr<llvm::Module> mymodule;
extern std::map<std::string, llvm::AllocaInst *> NamedValues;
extern std::unique_ptr<llvm::IRBuilder<>> builder;

namespace emitter {
  llvm::AllocaInst *make_alloca(llvm::Function *F, std::string name) {
    return nullptr;
  }

  llvm::Function *make_function(std::string name, std::vector<std::string> param_names) {
    return nullptr;
  }

  llvm::GlobalVariable *make_global(std::string name, double initval_d) {
    return nullptr;
  }
}
