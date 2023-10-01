
#include <string>
#include <vector>
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Function.h"

namespace emitter {
   llvm::Function *make_function(std::string name, std::vector<std::string> param_names);
   llvm::AllocaInst *make_alloca(std::string name);
   llvm::GlobalVariable *make_global(std::string name, double initval_d);
}
