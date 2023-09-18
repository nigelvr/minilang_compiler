#include <stdio.h>
#include <map>
#include <string>
#include <vector>
#include "ast.h"
#include "ex.tab.h"

extern FILE *yyin;
extern std::vector<AST *> ASTList;

int main(int argc, char **argv) {
    Environment env;

    printf("top of main\n");
    yyin = fopen("examples/test.txt", "r");
    if (!yyin) {
        fprintf(stderr, "failed to open input file\n");
        return 1;
    }
    int result = yyparse();
    printf("done parsing with result %d\n", result);
    printf("---------------------------\n\n");

    ASTList.at(0)->emit(env); // do the assignment
    ASTList.at(1)->emit(env); // do the funcdef

    printf("%d\n", ASTList.at(2)->emit(env)); // evaluate the expression, print it

    return result;
}