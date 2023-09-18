
#include <map>
#include <string>
#include <vector>

class AST;
class FuncDefAST;

class Environment {
public:
    Environment() {}
    Environment(std::map<std::string, int>, std::map<std::string, FuncDefAST *>);
    std::map<std::string, int> varenv;
    std::map<std::string, FuncDefAST *> funcenv;
};