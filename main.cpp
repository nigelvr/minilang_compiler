#include <stdio.h>
#include <map>
#include <string>
#include "ex.tab.h"

extern FILE *yyin;
extern std::map<std::string, int> Environment;

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

    printf("looping through env\n");
    auto it = Environment.begin();
    do {
        printf("(%s %d)\n", it->first.c_str(), it->second);
        ++it;
    } while (it != Environment.end());

    printf("access element a: %d\n", Environment["a"]);

    return result;
}