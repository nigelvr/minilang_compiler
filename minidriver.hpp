#ifndef __DRIVER_HPP__
#define __DRIVER_HPP__ 

#include <string>
#include <memory>
#include <cstddef>
#include <istream>

#include "miniscanner.hpp"
#include "miniparser.tab.hh"

#include "ast.h"

namespace MiniCompiler {

class Driver{
public:
   Driver(std::istream &in);
   virtual ~Driver() = default;
   int parse();
   void set_funcdef(std::shared_ptr<FuncDefAST> funcdef);
   llvm::Value *run_program();
private:
   std::shared_ptr<MiniCompiler::Parser> parser;
   std::shared_ptr<MiniCompiler::Scanner> scanner;
   std::shared_ptr<FuncDefAST> funcdef;
};

} 
#endif
