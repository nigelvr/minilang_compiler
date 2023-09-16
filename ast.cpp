#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "ast.h"
#include "env.h"

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

int BinOpAST::eval()
{
  switch(this->value.d) {
    case '+':
      return this->getl()->eval() + this->getr()->eval();
    case '-':
      return this->getl()->eval() - this->getr()->eval();
    case '*':
      return this->getl()->eval() * this->getr()->eval();
    case '/':
      return this->getl()->eval() / this->getr()->eval();
    /* case '=':
      printf("assignment %s = %d\n", this->getl()->value.s, this->getr()->eval());
      return -1000000; */
    case 'M':
      return -this->getl()->eval();
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

int AssignmentAST::getrval() {
  BinOpAST *binOp = (BinOpAST *)this->children.at(0);
  return binOp->eval();
}

int AssignmentAST::eval() {
  Environment[this->value.s] = this->getrval();
  return -1000000;
}