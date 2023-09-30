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
extern std::map<std::string, llvm::Value *> NamedValues;

namespace emitter {
  llvm::Function *make_function(std::string name, std::vector<std::string> param_names) {
    std::vector<llvm::Type *>Doubles(param_names.size(), llvm::Type::getDoubleTy(context));
    llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(context), Doubles, false);
    llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, name, mymodule.get());

    // set function arg names
    NamedValues.clear();
    int idx = 0;
    std::string param_name;
    for (auto &Arg : F->args()) {
      param_name = param_names.at(idx);
      Arg.setName(param_name);
      std::cout << "arg name = " << std::string(Arg.getName()) << std::endl;
      NamedValues[param_name] = &Arg;
      idx++;
    }
    
    return F;
  }

  llvm::GlobalVariable *make_global(std::string name, double initval_d) {
     mymodule->getOrInsertGlobal(name, llvm::Type::getDoubleTy(context));
     llvm::GlobalVariable *glob = mymodule->getNamedGlobal(name);
     glob->setLinkage(llvm::GlobalValue::CommonLinkage);
     glob->setAlignment(llvm::MaybeAlign(4));
     auto initval = llvm::ConstantFP::get(context, llvm::APFloat(initval_d));
     glob->setInitializer(initval);
     return glob;
  }
}
