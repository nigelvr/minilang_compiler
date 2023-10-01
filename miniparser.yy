%skeleton "lalr1.cc"
%require "3.2"
%define api.namespace {MiniCompiler}
%define api.parser.class {Parser}
%define api.value.type variant
%define parse.assert
%parse-param { Scanner  &scanner  }
%parse-param { Driver  &driver  }
%locations

%code requires{
#include <memory>
#include "ast.h"
namespace MiniCompiler {
   class Driver;
   class Scanner;
}
}

%code{
#include <iostream>
#include <cstdlib>
#include <fstream>
   
/* include for all driver functions */
#include "minidriver.hpp"

#undef yylex
#define yylex scanner.yylex
}

%token END 0 "end of file"
%token RETURN
%token FUNC
%token IF
%token <std::string> IDENT
%token <int>	     NUMBER

%type <std::shared_ptr<ExprAST>> exp
%type <std::shared_ptr<FuncDefAST>> funcdef
%type <std::vector<std::string>> param_names
%type <std::shared_ptr<StatementAST>> statement
%type <std::shared_ptr<StatementAST>> return_statment

%%
program:
       |
       funcdef {
          driver.set_funcdef($1);
       }
;

exp: exp '+' exp    { $$ = std::make_shared<BinOpAST>('+', $1, $3); }
   | exp '-' exp    { $$ = std::make_shared<BinOpAST>('-', $1, $3); }
   | exp '*' exp    { $$ = std::make_shared<BinOpAST>('*', $1, $3); }
   | exp '/' exp    { $$ = std::make_shared<BinOpAST>('/', $1, $3); }
   | exp '<' exp    { $$ = std::make_shared<BinOpAST>('<', $1, $3); }
   | '(' exp ')'    { $$ = $2; }
   | '-' exp        { $$ = std::make_shared<BinOpAST>('M', $2, nullptr); }
   | NUMBER         { $$ = std::make_shared<BinOpAST>($1, nullptr, nullptr); }
   | IDENT          { $$ = std::make_shared<VariableExprAST>($1); }

funcdef: FUNC IDENT '(' param_names ')' '{' statement '}' {
   $$ = std::make_shared<FuncDefAST>($2, $4, $7);
};

statement: return_statment { $$ = $1; };

return_statment: RETURN exp ';' {
   $$ = std::make_shared<ReturnAST>($2);
};

param_names: { $$ = std::vector<std::string>(); }
      | IDENT {
         std::vector<std::string> params; 
         params.push_back($1);
         $$ = params;
      }
      | param_names ',' IDENT {
         std::vector<std::string> params = $1;
         params.push_back($3);
         $$ = params;
      };
%%


void MiniCompiler::Parser::error( const location_type &l, const std::string &err_message )
{
   std::cerr << "Error: " << err_message << " at " << l << "\n";
}
