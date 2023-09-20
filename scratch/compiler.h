#include "env.h"
#include "ast.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

class Compiler {
public:
    Compiler();
    Environment compiler_env;
    std::vector<AST *> ASTList;
    LLVMContext context;
	Module *m;
	IRBuilder<> builder;
};