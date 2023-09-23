#include <stdio.h>
#include <map>
#include <string>
#include <vector>
#include "ast.h"
#include "ex.tab.h"
// LLVM
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"

extern FILE *yyin;
std::vector<AST *> ASTList;

llvm::LLVMContext context;
llvm::Module *mymodule;
llvm::IRBuilder<> builder(context);
std::map<std::string, llvm::Value *> NamedValues;

int main(int argc, char **argv) {
    mymodule = new llvm::Module("test", context);

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