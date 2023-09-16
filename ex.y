/* calculator with AST */

%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "ast.h"

extern int yylex();
%}

%union {
  struct BinOpAST *a;
  char *ident;
  int d;
}

/* declare tokens */
%token <d> NUMBER
%token <ident> IDENT
%token EOL

%left '+' '-'
%left '*' '/'
%nonassoc '|' UMINUS

%type <a> exp
%type <a> assignment
%type <a> progpart
%type <ident> assignment_lhs

%%
program:
       |
       program progpart EOL {
          printf("%d\n", $2->eval());
          // treefree($2);
       }
;

progpart: exp
        | assignment
        ;


exp: exp '+' exp { $$ = new BinOpAST(make_integer_value('+'), $1, $3); }
   | exp '-' exp { $$ = new BinOpAST(make_integer_value('-'), $1, $3); }
   | exp '*' exp { $$ = new BinOpAST(make_integer_value('*'), $1, $3); }
   | exp '/' exp { $$ = new BinOpAST(make_integer_value('/'), $1, $3); }
   | '(' exp ')' { $$ = $2; }
   | '-' exp     { $$ = new BinOpAST(make_integer_value('M'), $2, nullptr); }
   | NUMBER      { $$ = new BinOpAST(make_integer_value($1), nullptr, nullptr); }
 ;

 assignment: assignment_lhs '=' exp ';' {
  $$ = new BinOpAST(make_integer_value('='), nullptr, nullptr);
 };

 assignment_lhs: IDENT {
  $$ = $1;
 };

 %%

void yyerror(const char *s, ...)
{
  va_list ap;
  va_start(ap, s);

  fprintf(stderr, "%d: error: ", yylineno);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}