#include <cctype>
#include <cassert>
#include <fstream>
#include <memory>

#include "minidriver.hpp"

MiniCompiler::Driver::Driver(std::istream &stream)
{
	this->scanner = std::make_shared<MiniCompiler::Scanner>(&stream);
	this->parser = std::make_shared<MiniCompiler::Parser>(*(this->scanner), *this);
}

int MiniCompiler::Driver::parse()
{
   return parser->parse();
}


void MiniCompiler::Driver::add_word( const std::string &word )
{
   words++; 
}

void MiniCompiler::Driver::set_expression(std::shared_ptr<ExprAST> expr) {
   this->expr = expr;
}

std::ostream& MiniCompiler::Driver::print( std::ostream &stream )
{
   stream << "num words = " << words << std::endl;
   return(stream);
}
