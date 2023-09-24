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


extern FILE *yyin;
std::vector<AST *> ASTList;

llvm::LLVMContext context;
llvm::Module *mymodule;
llvm::IRBuilder<> builder(context);
std::map<std::string, llvm::Value *> NamedValues;
llvm::legacy::FunctionPassManager *FPM;

int main(int argc, char **argv) {
    mymodule = new llvm::Module("test", context);
    FPM = new llvm::legacy::FunctionPassManager(mymodule);

    FPM->add(llvm::createInstructionCombiningPass()); // peephole
    FPM->add(llvm::createReassociatePass());
    FPM->add(llvm::createGVNPass()); // eliminate common subexpressions
    FPM->add(llvm::createCFGSimplificationPass()); // simplify cfg
    FPM->doInitialization();


    printf("top of main\n");
    yyin = fopen("examples/test.txt", "r");
    if (!yyin) {
        fprintf(stderr, "failed to open input file\n");
        return 1;
    }
    int result = yyparse();
    if (result != 0) {
        fprintf(stderr, "Failed to parse file\n");
        return result;
    }
    
    // do the function definition
    llvm::Function *F = ((FuncDefAST *)ASTList.at(0))->emitllvm();
    // printf("llvm function definition\n");
    // F->print(llvm::errs());
    // printf("\n");
    // function call
    // llvm::Value *V = ((FuncCallAST *)ASTList.at(1))->emitllvm();
    llvm::Value *V = ((ExprAST *)ASTList.at(1))->emitllvm();
    
    // printf("function call\n");
    // V->print(llvm::errs());
    //printf("\n");
    
    mymodule->print(llvm::errs(), nullptr);

    return result;
}