%skeleton "lalr1.cc"
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

%%
program:
       |
       exp {
          driver.set_expression($1);
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


%%


void MiniCompiler::Parser::error( const location_type &l, const std::string &err_message )
{
   std::cerr << "Error: " << err_message << " at " << l << "\n";
}
