/* calculator with AST */

%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "ast.h"

extern int yylex();
%}

%union {
  struct ast *a;
  int d;
}

/* declare tokens */
%token <d> NUMBER
%token EOL

%left '+' '-'
%left '*' '/'
%nonassoc '|' UMINUS

%type <a> exp

%%
calclist: /* nothing */
| calclist exp EOL {
     printf("= %d\n", $2->eval());
     treefree($2);
     printf("> ");
 }

 | calclist EOL { printf("> "); } /* blank line or a comment */
 ;

exp: exp '+' exp { $$ = new ast('+', $1, $3); }
   | exp '-' exp { $$ = new ast('-', $1, $3); }
   | exp '*' exp { $$ = new ast('*', $1, $3); }
   | exp '/' exp { $$ = new ast('/', $1, $3); }
   | '(' exp ')' { $$ = $2; }
   | '-' exp     { $$ = new ast('M', $2, nullptr); }
   | NUMBER      { $$ = new ast($1, nullptr, nullptr); }
 ;
 %%

void yyerror(const char *s, ...)
{
  va_list ap;
  va_start(ap, s);

  fprintf(stderr, "%d: error: ", yylineno);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}

int main(int argc, char **argv)
{
  printf("> "); 
  return yyparse();
}