%{
#include <string>
#include <sstream>
#include <cstdlib>
#include "miniscanner.hpp"

#undef  YY_DECL
#define YY_DECL int MiniCompiler::Scanner::yylex( MiniCompiler::Parser::semantic_type * const lval, MiniCompiler::Parser::location_type *loc )

/* typedef to make the returns for the tokens shorter */
using token = MiniCompiler::Parser::token;

/* update location on matching */
#define YY_USER_ACTION loc->step(); loc->columns(yyleng);

using namespace std;

%}

%option nodefault
%option yyclass="MiniCompiler::Scanner"
%option noyywrap
%option c++

%%
%{          /** Code executed at the beginning of yylex **/
            yylval = lval;
%}

"+" |
"-" |
"*" |
"/" |
"(" |
"{" |
"}" |
"=" |
";" |
"," |
"<" |
")"     { return yytext[0]; }

"return" { return token::RETURN; }
"func" { return token::FUNC; }
"if" { return token::IF; }

[a-zA-Z_][a-zA-Z_0-9]* {
	yylval->build<std::string>(yytext);
	return token::IDENT;
}

[0-9]+ {
	yylval->build<int>(atoi(yytext));
	return token::NUMBER;
}

\n {
        loc->lines();
}

. {
	std::ostringstream err;
	err << "Unknown character: " << yytext << std::endl;
	lexer_error(err.str());
}

"//".*  {}

[ \t]   {}

%%

