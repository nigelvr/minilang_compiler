#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <map>
#include <string>
#include "ast.h"

std::vector<AST *> ASTList;

/**
 * Language underlying values
*/
Value make_integer_value(int d) {
  Value v;
  v.vt = ValueType::Integer;
  v.d = d;
  return v;
}

Value make_string_value(char *s) {
  Value v;
  v.vt = ValueType::String;
  v.s = s;
  return v;
}

/**
 * Binary Operators
*/
BinOpAST::BinOpAST(Value value, ExpressionAST *l, ExpressionAST *r) {
  this->value = value;
  this->children.push_back(l);
  this->children.push_back(r);
}

BinOpAST *BinOpAST::getl() {
  return (BinOpAST *)children.at(0);
}

BinOpAST *BinOpAST::getr() {
  return (BinOpAST *)children.at(1);
}

int BinOpAST::eval(Environment& env)
{
  if (this->value.vt == ValueType::String) {
    return env.varenv[std::string(this->value.s)];
  }
  switch(this->value.d) {
    case '+':
      return this->getl()->eval(env) + this->getr()->eval(env);
    case '-':
      return this->getl()->eval(env) - this->getr()->eval(env);
    case '*':
      return this->getl()->eval(env) * this->getr()->eval(env);
    case '/':
      return this->getl()->eval(env) / this->getr()->eval(env);
    case 'M':
      return -this->getl()->eval(env);
  }
  return this->value.d;
}

void treefree(BinOpAST *a)
{
  int val = a->value.d;
  if (val == '+' || val == '-' || val == '*' || val == '/') {
    treefree(a->getl());
    treefree(a->getr());
  }
  else if (val == 'M') {
    treefree(a->getl());
  }
  else {
    delete a;
  }
}

/**
 * Assignments
*/
AssignmentAST::AssignmentAST(Value value, ExpressionAST *a) {
  this->value = value;
  this->children.push_back(a);
}

int AssignmentAST::getrval(Environment& env) {
  BinOpAST *binOp = (BinOpAST *)this->children.at(0);
  return binOp->eval(env);
}

int AssignmentAST::eval(Environment& env) {
  std::string key(this->value.s);
  int val = this->getrval(env);
  env.varenv[key] = val;
  return -1000000;
}

/**
 * Function definition
 */
FuncDefAST::FuncDefAST(Value value, std::vector<std::string> param_list, ExpressionAST *a) {
  this->value = value;
  this->param_list = param_list;
  this->children.push_back(a);
}

int FuncDefAST::eval(Environment &env) {
  std::string key(this->value.s);
  env.funcenv[key] = this;
  return -1000000;
}

/**
 * Function calls
*/
FuncCallAST::FuncCallAST(Value value, std::vector<int> arg_list) {
  this->value = value;
  this->arg_list = arg_list;
}

int FuncCallAST::eval(Environment &env) {
  FuncDefAST *f = env.funcenv[this->value.s];
  auto expr = f->children.at(0);
  return expr->eval(env);
}