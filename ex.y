/* calculator with AST */

%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <vector>
#include "ast.h"

extern std::vector<AST *>ASTList;
extern int yylex();
%}

%union {
  struct AST *g_ast;
  struct BinOpAST *b_ast;
  struct AssignmentAST *a_ast;
  struct FuncDefAST *fdef_ast;
  struct FuncCallAST *fcall_ast;
  char *ident;
  int d;
}

/* declare tokens */
%token RETURN
%token FUNC
%token <d> NUMBER
%token <ident> IDENT

%left '+' '-'
%left '*' '/'
%nonassoc '|' UMINUS

%type <g_ast> exp
%type <a_ast> assignment
%type <fdef_ast> funcdef


%%
program:
       |
       assignment funcdef exp {
          ASTList.push_back((AST *)$1); // assignment
          ASTList.push_back((AST *)$2); // funcdef
          ASTList.push_back((AST *)$3); // expression
       }
;

exp: exp '+' exp    { $$ = new BinOpAST(make_integer_value('+'), $1, $3); }
   | exp '-' exp    { $$ = new BinOpAST(make_integer_value('-'), $1, $3); }
   | exp '*' exp    { $$ = new BinOpAST(make_integer_value('*'), $1, $3); }
   | exp '/' exp    { $$ = new BinOpAST(make_integer_value('/'), $1, $3); }
   | '(' exp ')'    { $$ = $2; }
   | '-' exp        { $$ = new BinOpAST(make_integer_value('M'), $2, nullptr); }
   | NUMBER         { $$ = new BinOpAST(make_integer_value($1), nullptr, nullptr); }
   | IDENT          { $$ = new BinOpAST(make_string_value($1), nullptr, nullptr); }
   | IDENT '(' ')'  { $$ = new FuncCallAST(make_string_value($1), std::vector<int>()); }
 ;

 assignment: IDENT '=' exp ';' {
  $$ = new AssignmentAST(make_string_value($1), $3);
 };

 funcdef: FUNC IDENT '(' ')' '{' RETURN exp ';' '}' {
  $$ = new FuncDefAST(make_string_value($2), std::vector<std::string>(), $7);
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