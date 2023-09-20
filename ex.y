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
  struct ExpressionAST *e_ast;
  struct AssignmentAST *a_ast;
  struct FuncDefAST *fdef_ast;
  struct FuncCallAST *fcall_ast;
  struct ParamList *plist_h;
  struct ArgList *arglist_h;
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

%type <e_ast> exp
%type <a_ast> assignment
%type <fdef_ast> funcdef
%type <plist_h> params
%type <arglist_h> args


%%
program:
       |
       assignment funcdef exp {
          ASTList.push_back(static_cast<AST *>($1)); // assignment
          ASTList.push_back(static_cast<AST *>($2)); // funcdef
          ASTList.push_back(static_cast<AST *>($3)); // expression
       }
;

exp: exp '+' exp    { $$ = new BinOpAST('+', $1, $3); }
   | exp '-' exp    { $$ = new BinOpAST('-', $1, $3); }
   | exp '*' exp    { $$ = new BinOpAST('*', $1, $3); }
   | exp '/' exp    { $$ = new BinOpAST('/', $1, $3); }
   | '(' exp ')'    { $$ = $2; }
   | '-' exp        { $$ = new BinOpAST('M', $2, nullptr); }
   | NUMBER         { $$ = new BinOpAST($1, nullptr, nullptr); }
   | IDENT          { $$ = new BinOpAST($1, nullptr, nullptr); }
   | IDENT '(' args ')'  { $$ = new FuncCallAST($1, $3); }
 ;

 assignment: IDENT '=' exp ';' {
  $$ = new AssignmentAST($1, $3);
 };

 funcdef: FUNC IDENT '(' params ')' '{' RETURN exp ';' '}' {
  $$ = new FuncDefAST($2, $4, $8);
 };

 params: { new ParamList(); }
        | IDENT { 
            ParamList *p = new ParamList();
            p->params.push_back($1);
            $$ = p;
        }
        | params ',' IDENT {
          auto a1 = $1;
          a1->params.push_back($3);
          $$ = a1;
        };

 args: { new ArgList(); }
     | NUMBER {
          ArgList *a = new ArgList();
          a->args.push_back($1);
          $$ = a;
     }
     | args ',' NUMBER {
          auto a1 = $1;
          a1->args.push_back($3);
          $$ = a1;
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