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

   void add_word(const std::string &word);

   void set_expression(std::shared_ptr<ExprAST> e);

   std::ostream& print(std::ostream &stream);
private:
   std::shared_ptr<MiniCompiler::Parser> parser;
   std::shared_ptr<MiniCompiler::Scanner> scanner;
   std::shared_ptr<ExprAST> expr;
   std::size_t words = 0;
   std::size_t lines = 0;
};

} 
#endif
