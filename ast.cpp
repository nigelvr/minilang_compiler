#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "ast.h"
#include "env.h"

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
    case '=':
      printf("assignement to %d\n", this->r->eval());
      return 0;
    case 'M':
      return -this->l->eval();
  }
  return this->value;
}

void treefree(ast *a)
{
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