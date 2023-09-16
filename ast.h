#include <vector>
#include <map>
#include <string>

/* interface to the lexer */
extern int yylineno; /* from lexer */
void yyerror(const char *, ...);

enum class ValueType { String, Integer };

struct Value {
    ValueType vt;
    union {
        char *s;
        int d;
    };
};

Value make_integer_value(int);
Value make_string_value(char *);

class AST {
public:
  Value value;
  std::vector<AST *> children;
  virtual int eval(std::map<std::string, int>&) = 0;
};

class BinOpAST : public AST {
public:
  BinOpAST(Value, BinOpAST *, BinOpAST *);
  int eval(std::map<std::string, int>&);
  BinOpAST *getl();
  BinOpAST *getr();
};

class AssignmentAST : public AST {
public:
  AssignmentAST(Value, BinOpAST *);
  int getrval(std::map<std::string, int>&);
  int eval(std::map<std::string, int>&);
};

/**
 * Misc
*/
/* delete and free an AST */
// XXX hack
void treefree(BinOpAST *);