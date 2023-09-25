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
  struct ExprAST *e_ast;
  struct FuncDefAST *fdef_ast;
  struct FuncCallAST *fcall_ast;
  struct ParamList *plist_h;
  struct ArgList *alist_h;
  struct FuncPart *funcpart_h;
  struct FuncPartList *fpart_l;
  char *ident;
  int d;
}

/* declare tokens */
%token RETURN
%token FUNC
%token IF
%token <d> NUMBER
%token <ident> IDENT

%left '<'
%left '+' '-'
%left '*' '/'
%nonassoc '|' UMINUS

%type <e_ast> exp
%type <fdef_ast> funcdef
%type <funcpart_h> funcpart
%type <plist_h> params
%type <alist_h> args


%%
program:
       |
       funcdef exp {
          ASTList.push_back(static_cast<AST *>($1)); // funcdef
          ASTList.push_back(static_cast<AST *>($2)); // expression
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
   | IDENT '(' args ')' { $$ = new FuncCallAST($1, $3); }
 ;

funcdef: FUNC IDENT '(' params ')' '{' funcpart ';' '}' {
  $$ = new FuncDefAST($2, $4, $7);
};

funcpart: RETURN exp {
  $$ = new FuncPart(FuncPartType::Return, (ExprAST *)$2, nullptr, nullptr);
  }
  |
  IF '(' exp ')' '{' funcpart '}' {
    $$ = new FuncPart(FuncPartType::If, nullptr, $3, $6);
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