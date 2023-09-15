#include <stdio.h>
#include "ex.tab.h"

extern FILE *yyin;

int main(int argc, char **argv) {
    yyin = fopen("examples/test.txt", "r");
    if (!yyin) {
        fprintf(stderr, "failed to open input file\n");
        return 1;
    }
    return yyparse();
}