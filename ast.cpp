#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "ast.h"

ast::ast(int value, ast *l, ast *r) {
  this->value = value;
  this->l = l;
  this->r = r;
}

int ast::eval()
{
  switch(this->value) {
    case '+':
      return this->l->eval() + this->r->eval();
    case '-':
      return this->l->eval() - this->r->eval();
    case '*':
      return this->l->eval() * this->r->eval();
    case '/':
      return this->l->eval() / this->r->eval();
    case 'M':
      return -this->l->eval();
  }
  return this->value;
}

void treefree(ast *a)
{
  printf("freeing tree of type/value %c\n", a->value);

  int val = a->value;
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