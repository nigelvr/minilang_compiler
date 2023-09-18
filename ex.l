/* recognize tokens for the calculator */
%option noyywrap nodefault yylineno
%{
#include <string.h>
#include "ast.h"
#include "ex.tab.h"
%}

/* float exponent */
EXP     ([Ee][-+]?[0-9]+)

%%
"+" |
"-" |
"*" |
"/" |
"(" |
"{" |
"}" |
"=" |
";" |
"," |
")"     { return yytext[0]; }
[0-9]+ { yylval.d = atoi(yytext); return NUMBER; }

"return" { return RETURN; }
"func" { return FUNC; }

[a-zA-Z_][a-zA-Z_0-9]* {
    yylval.ident = strdup(yytext);
    return IDENT;
}


"//".*  
[ \t\n]   { /* ignore whitespace */ }
.       { yyerror("Mystery character %c\n", *yytext); }
%%