#ifndef __SCANNER_H__
#define __SCANNER_H__

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include <cstdlib>
#include "miniparser.tab.hh"
#include "location.hh"

namespace MiniCompiler {

class Scanner : public yyFlexLexer{
public:
   Scanner(std::istream *in) : yyFlexLexer(in) {};
   virtual ~Scanner() {};

   using yyFlexLexer::yylex;
   virtual int yylex(MiniCompiler::Parser::semantic_type* const lval, MiniCompiler::Parser::location_type *location);
   
   void lexer_error(std::string s) {
	std::cerr << "Lexer error: " << s << std::endl;
	exit(1);	
   }

private:
   MiniCompiler::Parser::semantic_type *yylval = nullptr;
};

} 

#endif
