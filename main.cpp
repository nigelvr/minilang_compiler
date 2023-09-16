#include <stdio.h>
#include <map>
#include <string>
#include <vector>
#include "ast.h"
#include "ex.tab.h"

extern FILE *yyin;
extern std::vector<AST *> ASTList;
std::map<std::string, int> Environment;

int main(int argc, char **argv) {
    printf("top of main\n");
    yyin = fopen("examples/test.txt", "r");
    if (!yyin) {
        fprintf(stderr, "failed to open input file\n");
        return 1;
    }
    int result = yyparse();
    printf("done parsing with result %d\n", result);
    printf("---------------------------\n\n");

    ASTList.at(0)->eval(Environment); // do assignment
    printf("%d\n", ASTList.at(1)->eval(Environment)); // do expression, print it

    return result;
}