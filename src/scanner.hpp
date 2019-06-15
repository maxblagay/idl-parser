#pragma once

#include "Node.hpp"
#include "ParserDriver.hpp"

#define YYSTYPE ast::Node *
#define IDLSTYPE ast::Node *

#include "simple_parser.hpp"

//#ifndef __FLEX_LEXER_H
#undef yyFlexLexer
//#define yyFlexLexer IdlFlexLexer
#include <FlexLexer.h>
#undef yyFlexLexer
#undef yyFlexLexerOnce

#ifndef YY_DECL

#define YY_DECL \
    idl::Parser::token_type idl::Scanner::lex( \
        idl::Parser::semantic_type *yylval, \
        idl::Parser::location_type *yylloc, \
        ParserDriver &driver)
#endif

struct ParserDriver;

namespace idl
{
class Scanner : public yyFlexLexer
{
public:
    Scanner(std::istream *arg_yyin = nullptr, std::ostream *arg_yyout = nullptr);
    virtual ~Scanner();
    virtual Parser::token_type
    lex(Parser::semantic_type *yylval, Parser::location_type *yylloc, ParserDriver &driver);
};
} // namespace idl

//#endif
