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
  struct BinOpAST *bop_ast;
  struct FuncDefAST *fdef_ast;
  struct FuncCallAST *fcall_ast;
  struct ParamList *plist_h;
  struct ArgList *alist_h;
  char *ident;
  int d;
}

/* declare tokens */
%token RETURN
%token FUNC
%token <d> NUMBER
%token <ident> IDENT

%left '<'
%left '+' '-'
%left '*' '/'
%nonassoc '|' UMINUS

%type <bop_ast> exp
%type <fdef_ast> funcdef
%type <plist_h> params
%type <fcall_ast> funccall
%type <alist_h> args


%%
program:
       |
       funcdef funccall {
          ASTList.push_back(static_cast<AST *>($1)); // funcdef
          ASTList.push_back(static_cast<AST *>($2)); // funccall
       }
;

exp: exp '+' exp    { $$ = new BinOpAST('+', $1, $3); }
   | exp '-' exp    { $$ = new BinOpAST('-', $1, $3); }
   | exp '*' exp    { $$ = new BinOpAST('*', $1, $3); }
   | exp '/' exp    { $$ = new BinOpAST('/', $1, $3); }
   | exp '<' exp    { $$ = new BinOpAST('<', $1, $3); }
   | '(' exp ')'    { $$ = $2; }
   | '-' exp        { $$ = new BinOpAST('M', $2, nullptr); }
   | NUMBER         { $$ = new BinOpAST($1, nullptr, nullptr); }
   | IDENT          { $$ = new BinOpAST($1, nullptr, nullptr); }
 ;

funcdef: FUNC IDENT '(' params ')' '{' RETURN exp ';' '}' {
  $$ = new FuncDefAST($2, $4, (BinOpAST *)$8);
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

funccall: IDENT '(' args ')' ';' {
  $$ = new FuncCallAST($1, $3);
}

args: { new ArgList(); }
    | exp {
      ArgList *l = new ArgList();
      l->args.push_back($1);
      $$ = l;
    }
    | args ',' exp {
      ArgList *l = $1;
      l->args.push_back($3);
      $$ = l;
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