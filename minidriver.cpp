#include <cctype>
#include <cassert>
#include <fstream>
#include <memory>

#include "minidriver.hpp"

MiniCompiler::Driver::Driver(std::istream &stream) {
	this->scanner = std::make_shared<MiniCompiler::Scanner>(&stream);
	this->parser = std::make_shared<MiniCompiler::Parser>(*(this->scanner), *this);
}

int MiniCompiler::Driver::parse() {
   return parser->parse();
}

void MiniCompiler::Driver::set_funcdef(std::shared_ptr<FuncDefAST> funcdef) {
   this->funcdef = funcdef;
}

llvm::Value *MiniCompiler::Driver::compile_program() {
   return this->funcdef->emitllvm();
}
