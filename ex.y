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
  struct BinOpAST *b_ast;
  struct AssignmentAST *a_ast;
  char *ident;
  int d;
}

/* declare tokens */
%token <d> NUMBER
%token <ident> IDENT

%left '+' '-'
%left '*' '/'
%nonassoc '|' UMINUS

%type <b_ast> exp
%type <a_ast> assignment

%%
program:
       |
       assignment exp {
          // do the assignment
          $1->eval();
          // print the expression value
          printf("%d\n", $2->eval());
       }
;

exp: exp '+' exp { $$ = new BinOpAST(make_integer_value('+'), $1, $3); }
   | exp '-' exp { $$ = new BinOpAST(make_integer_value('-'), $1, $3); }
   | exp '*' exp { $$ = new BinOpAST(make_integer_value('*'), $1, $3); }
   | exp '/' exp { $$ = new BinOpAST(make_integer_value('/'), $1, $3); }
   | '(' exp ')' { $$ = $2; }
   | '-' exp     { $$ = new BinOpAST(make_integer_value('M'), $2, nullptr); }
   | NUMBER      { $$ = new BinOpAST(make_integer_value($1), nullptr, nullptr); }
   | IDENT       { $$ = new BinOpAST(make_string_value($1), nullptr, nullptr); }
 ;

 assignment: IDENT '=' exp ';' {
  $$ = new AssignmentAST(make_string_value($1), $3);
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