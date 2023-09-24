#include <stdio.h>
#include <map>
#include <string>
#include <vector>
#include "ast.h"
#include "ex.tab.h"
// Kaleidoscope JIT
#include "KaleidoscopeJIT.h"
// LLVM library
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
// function pass
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"


// Parser globals
extern FILE *yyin;
std::vector<AST *> ASTList;

// Compiler globals
llvm::LLVMContext context;
llvm::Module *mymodule;
llvm::IRBuilder<> builder(context);
std::map<std::string, llvm::Value *> NamedValues;
llvm::legacy::FunctionPassManager *FPM;

void init_compiler_objects() {
    mymodule = new llvm::Module("test", context);
    FPM = new llvm::legacy::FunctionPassManager(mymodule);

    FPM->add(llvm::createInstructionCombiningPass()); // peephole
    FPM->add(llvm::createReassociatePass());
    FPM->add(llvm::createGVNPass()); // eliminate common subexpressions
    FPM->add(llvm::createCFGSimplificationPass()); // simplify cfg
    FPM->doInitialization();
}

int parse_file(const std::string path) {
    yyin = fopen(path.c_str(), "r");
    if (!yyin) {
        fprintf(stderr, "failed to open input file\n");
        return 1;
    }
    return yyparse();
}

void driver() {
    llvm::Function *F = ((FuncDefAST *)ASTList.at(0))->emitllvm();
    llvm::Value *V = ((ExprAST *)ASTList.at(1))->emitllvm();
    mymodule->print(llvm::errs(), nullptr);
}

int main(int argc, char **argv) {
    init_compiler_objects();

    int parse_result = parse_file("examples/test.txt");
    if (parse_result) {
        fprintf(stderr, "failed to open input file\n");
        return parse_result;
    }

    driver();

    return parse_result;
}