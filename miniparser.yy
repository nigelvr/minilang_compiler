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

%left '='
%left '<'
%left '+' '-'
%left '*' '/'
%left UMINUS

%token END 0 "end of file"
%token RETURN
%token FUNC
%token ELSE
%token IF
%token <std::string> IDENT
%token <int>	     NUMBER

%type <std::shared_ptr<ExprAST>> exp
%type <std::shared_ptr<FuncDefAST>> funcdef
%type <std::vector<std::string>> param_names
%type <std::shared_ptr<StatementAST>> statement
%type <std::vector<std::shared_ptr<StatementAST>>> statement_block
%type <std::shared_ptr<StatementAST>> return_statment
%type <std::shared_ptr<StatementAST>> branch_statment
%type <std::shared_ptr<AssignmentAST>> assignment_statement
%type <std::vector<std::shared_ptr<ExprAST>> param_values

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
   | '-' exp %prec UMINUS        { $$ = std::make_shared<BinOpAST>('M', $2, nullptr); }
   | NUMBER         { $$ = std::make_shared<BinOpAST>($1, nullptr, nullptr); }
   | IDENT          { $$ = std::make_shared<VariableExprAST>($1); }
   | IDENT '(' param_values ')' { $$}

param_values: { $$ = std::vector<std::shared_ptr<ExprAST>>(); }
      | exp {
         std::vector<std::shared_ptr<ExprAST>> params; 
         params.push_back($1);
         $$ = params;
      }
      | param_values ',' exp {
         std::vector<std::shared_ptr<ExprAST>> params = $1;
         params.push_back($3);
         $$ = params;
      };

funcdef: FUNC IDENT '(' param_names ')' '{' statement_block '}' {
   std::cout << "constructing function: " << $2 << std::endl;
   $$ = std::make_shared<FuncDefAST>($2, $4, $7);
};

statement_block: statement {
                  std::vector<std::shared_ptr<StatementAST>> statements;
                  statements.push_back($1);
                  $$ = statements;
               }
               | statement_block statement {
                  std::vector<std::shared_ptr<StatementAST>> statements = $1;
                  statements.push_back($2);
                  $$ = statements;
               };

statement: return_statment { $$ = $1; }
         | branch_statment { $$ = $1; }
         | assignment_statement { $$ = $1; }

return_statment: RETURN exp ';' {
   $$ = std::make_shared<ReturnAST>($2);
};

branch_statment: IF '(' exp ')' '{' statement '}' {
   std::cout << "constructing simple if" << std::endl;
   $$ = std::make_shared<BranchAST>($3, $6, nullptr);
   }
   |
   IF '(' exp ')' '{' statement '}' ELSE '{' statement '}' {
      std::cout << "constructing if else" << std::endl;
      $$ = std::make_shared<BranchAST>($3, $6, $10);
   };

assignment_statement: IDENT '=' exp ';' {
   $$ = std::make_shared<AssignmentAST>($1, $3);
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
