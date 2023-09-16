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
BinOpAST::BinOpAST(Value value, BinOpAST *l, BinOpAST *r) {
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

int BinOpAST::eval(std::map<std::string, int>& env)
{
  if (this->value.vt == ValueType::String) {
    return env[std::string(this->value.s)];
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
AssignmentAST::AssignmentAST(Value value, BinOpAST *a) {
  this->value = value;
  this->children.push_back(a);
}

int AssignmentAST::getrval(std::map<std::string, int>& env) {
  BinOpAST *binOp = (BinOpAST *)this->children.at(0);
  return binOp->eval(env);
}

int AssignmentAST::eval(std::map<std::string, int>& env) {
  std::string key(this->value.s);
  int val = this->getrval(env);
  env[key] = val;
  return -1000000;
}