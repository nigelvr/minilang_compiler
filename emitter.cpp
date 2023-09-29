#include "llvm/ADT/APInt.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IR/LegacyPassManager.h"

extern llvm::LLVMContext context;
extern std::unique_ptr<llvm::Module> mymodule;
// std::unique_ptr<llvm::IRBuilder<>> builder;
// std::unique_ptr<llvm::legacy::FunctionPassManager> FPM;

namespace emitter {
  llvm::Function *make_function(std::string name, std::vector<std::string> param_names) {
    std::vector<llvm::Type *>Doubles(0, llvm::Type::getDoubleTy(context));
    llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(context), Doubles, false);
    return llvm::Function::Create(FT, llvm::Function::ExternalLinkage, name, mymodule.get());
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
