#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <map>
#include <string>
#include "ast.h"

/**
 * Envrionment
*/
Environment::Environment(std::map<std::string, int>vars, std::map<std::string, FuncDefAST *>funcs) {
  this->varenv = vars;
  this->funcenv = funcs;
}

/**
 * Language underlying values
*/
UValue make_integer_value(int d) {
  UValue v;
  v.vt = UValueType::Integer;
  v.d = d;
  return v;
}

UValue make_string_value(char *s) {
  UValue v;
  v.vt = UValueType::String;
  v.s = s;
  return v;
}

/**
 * Binary Operators
*/
BinOpAST::BinOpAST(int value, ExpressionAST *l, ExpressionAST *r) {
  this->value = make_integer_value(value);
  this->children.push_back(l);
  this->children.push_back(r);
}

BinOpAST::BinOpAST(char *value, ExpressionAST *l, ExpressionAST *r) {
  this->value = make_string_value(value);
  this->children.push_back(l);
  this->children.push_back(r);
}

BinOpAST *BinOpAST::getl() {
  return (BinOpAST *)children.at(0);
}

BinOpAST *BinOpAST::getr() {
  return (BinOpAST *)children.at(1);
}

int BinOpAST::emit(Environment& env)
{
  if (this->value.vt == UValueType::String) {
    return env.varenv[std::string(this->value.s)];
  }
  switch(this->value.d) {
    case '+':
      return this->getl()->emit(env) + this->getr()->emit(env);
    case '-':
      return this->getl()->emit(env) - this->getr()->emit(env);
    case '*':
      return this->getl()->emit(env) * this->getr()->emit(env);
    case '/':
      return this->getl()->emit(env) / this->getr()->emit(env);
    case 'M':
      return -this->getl()->emit(env);
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
AssignmentAST::AssignmentAST(char *value, ExpressionAST *a) {
  this->value = make_string_value(value);
  this->children.push_back(a);
}

int AssignmentAST::getrval(Environment& env) {
  BinOpAST *binOp = (BinOpAST *)this->children.at(0);
  return binOp->emit(env);
}

int AssignmentAST::emit(Environment& env) {
  std::string key(this->value.s);
  int val = this->getrval(env);
  env.varenv[key] = val;
  return -1000000;
}

/**
 * Function definition
 */
FuncDefAST::FuncDefAST(char *value, ParamList *param_list, ExpressionAST *a) {
  this->value = make_string_value(value);
  this->param_list = param_list;
  this->children.push_back(a);
}

int FuncDefAST::emit(Environment &env) {
  std::string key(this->value.s);
  env.funcenv[key] = this;
  printf("function %s defined with params: ", this->value.s);
  for (int i = 0; i < this->param_list->params.size(); i++) {
    printf("%s ", this->param_list->params.at(i).c_str());
  }
  printf("\n");
  return -1000000;
}

/**
 * Function calls
*/
FuncCallAST::FuncCallAST(char *value, ArgList *arglist) {
  this->value = make_string_value(value);
  this->arglist = arglist;
}

int FuncCallAST::emit(Environment &env) {
  Environment tmpenv;
  tmpenv.varenv = env.varenv;
  tmpenv.funcenv = env.funcenv;
  FuncDefAST *f = tmpenv.funcenv[this->value.s];
  printf("Calling function %s with args: \n", this->value.s);

  // copy params into tmpenv
  for (int i = 0; i < f->param_list->params.size(); i++) {
    std::string key = f->param_list->params.at(i);
    int value = this->arglist->args.at(i);
    tmpenv.varenv[key] = value;
    printf("(%s %d)\n", key.c_str(), value);
  }

  
  auto expr = f->children.at(0);
  return expr->emit(tmpenv);
}