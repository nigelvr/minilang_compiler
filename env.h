
#include <map>
#include <string>
#include <vector>

class AST;

class Environment {
public:
    Environment() {}
    std::map<std::string, int> varenv;
    std::map<std::string, std::vector<AST *>> funcenv;
};