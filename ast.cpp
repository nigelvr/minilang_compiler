#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "ast.h"
#include "env.h"

ast::ast(Value value, ast *l, ast *r) {
  this->value = value;
  this->l = l;
  this->r = r;
}

int ast::eval()
{
  switch(this->value.d) {
    case '+':
      return this->l->eval() + this->r->eval();
    case '-':
      return this->l->eval() - this->r->eval();
    case '*':
      return this->l->eval() * this->r->eval();
    case '/':
      return this->l->eval() / this->r->eval();
    case '=':
      printf("assignment %s = %d\n", this->l->value.s, this->r->eval());
      return 0;
    case 'M':
      return -this->l->eval();
  }
  return this->value.d;
}

void treefree(ast *a)
{
  int val = a->value.d;
  if (val == '+' || val == '-' || val == '*' || val == '/') {
    treefree(a->l);
    treefree(a->r);
  }
  else if (val == 'M') {
    treefree(a->l);
  }
  else {
    delete a;
  }
}

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