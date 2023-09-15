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
  struct ast *a;
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
%type <a> assignment_lhs

%%
calclist: /* nothing */
| calclist exp EOL {
     printf("%d\n", $2->eval());
     treefree($2);
 }
 | calclist assignment EOL {
    $2->eval();
    treefree($2);
 }

 | calclist EOL { printf("> "); } /* blank line or a comment */
 ;

exp: exp '+' exp { $$ = new ast(make_integer_value('+'), $1, $3); }
   | exp '-' exp { $$ = new ast(make_integer_value('-'), $1, $3); }
   | exp '*' exp { $$ = new ast(make_integer_value('*'), $1, $3); }
   | exp '/' exp { $$ = new ast(make_integer_value('/'), $1, $3); }
   | '(' exp ')' { $$ = $2; }
   | '-' exp     { $$ = new ast(make_integer_value('M'), $2, nullptr); }
   | NUMBER      { $$ = new ast(make_integer_value($1), nullptr, nullptr); }
 ;

 assignment: assignment_lhs '=' exp ';' {
  $$ = new ast(make_integer_value('='), $1, $3);
 }

 assignment_lhs: IDENT {
  $$ = new ast(make_string_value($1), nullptr, nullptr);
 }

 %%

void yyerror(const char *s, ...)
{
  va_list ap;
  va_start(ap, s);

  fprintf(stderr, "%d: error: ", yylineno);
  vfprintf(stderr, s, ap);
  fprintf(stderr, "\n");
}