%option noyywrap nounput noinput batch debug
%option c++

%{

#include "Parser.hpp"
#include "scanner.hpp"

typedef idl::Parser::token token;
typedef idl::Parser::token_type token_type;

using namespace ast;

#define yyterminate() return token::END

#undef YY_NULL
#define YY_NULL nullptr


%}


%%

%{
    // reset location
    yylloc->step();
%}

[ \t]   {
            yylloc->step();
        }

"short" {
            *yylval = new NodeType(IdlType::Short);
            return token::SHORT;
        }

"long"  {
            *yylval = new NodeType(IdlType::Long);
            return token::LONG;
        }

"float" {
            *yylval = new NodeType(IdlType::Float);
            return token::FLOAT;
        }

"double" {
            *yylval = new NodeType(IdlType::Double);
            return token::DOUBLE;
        }

"char" {
            *yylval = new NodeType(IdlType::Char);
            return token::CHAR;
        }

"wchar" {
            *yylval = new NodeType(IdlType::Char);
            return token::WCHAR;
        }

"boolean" {
            *yylval = new NodeType(IdlType::Boolean);
            return token::BOOLEAN;
        }

"any" {
            *yylval = new NodeType(IdlType::Any);
            return token::ANY;
        }

"octet" {
            *yylval = new NodeType(IdlType::Octet);
            return token::OCTET;
        }

"void" {
            *yylval = new NodeType(IdlType::Void);
            return token::VOID;
        }

"string" {
            *yylval = new NodeType(IdlType::String);
            return token::STRING;
        }

"wstring" {
            *yylval = new NodeType(IdlType::WString);
            return token::WSTRING;
        }

"Object" {
            *yylval = new NodeType(IdlType::Object);
            return token::OBJECT;
        }

"module"    { return token::MODULE; }

"interface" { return token::INTERFACE; }

"struct"    { return token::STRUCT; }

"union"     { return token::UNION; }

"switch"    { return token::SWITCH; }

"case"      { return token::CASE; }

"default"   { return token::DEFAULT; }

"enum"      { return token::ENUM; }

"typedef"   { return token::TYPEDEF; }

"sequence"  { return token::SEQUENCE; }

"readonly"  { return token::READONLY; }

"unsigned"  { return token::UNSIGNED; }

"attribute" { return token::ATTRIBUTE; }

"const"     { return token::CONST; }

"exception" { return token::EXCEPTION; }

"native"    { return token::NATIVE; }

"raises"    { return token::RAISES; }

"abstract" {
            *yylval = new Node(Type::Abstract);
            return token::ABSTRACT;
        }

"local" {
            *yylval = new Node(Type::Local);
            return token::LOCAL;
        }


"in"    {
            *yylval = new ParamAttr(ParamDirection::In);
            return token::IN;
        }

"out"   {
            *yylval = new ParamAttr(ParamDirection::Out);
            return token::OUT;
        }

"inout" {
            *yylval = new ParamAttr(ParamDirection::InOut);
            return token::INOUT;
        }

"oneway" {
            *yylval = new Node(Type::OneWay);
            return token::ONEWAY;
        }

"fixed" {
            *yylval = new Node(Type::Fixed);
            return token::FIXED;
        }

"ValueBased" {
            *yylval = new Node(Type::ValueBased);
            return token::VALUEBASED;
        }

"TRUE" {
            *yylval = new Literal(yytext);
            return token::TRUE;
        }

"FALSE" {
            *yylval = new Literal(yytext);
            return token::FALSE;
        }

"exit"  {return token::QUIT;}
"quit"  {return token::QUIT;}

[0-9]+  {
            *yylval = new Literal(yytext);
            return token::INTEGER;
        }

[0-9a-zA-Z_]+  {
            *yylval = new Name(yytext);
            return token::NAME;
        }


\n		{
            yylloc->lines(yyleng);
            yylloc->step();
        }

"::"    {return token::SCOPE;}
"<<"    {return token::LEFT_SHIFT;}
">>"    {return token::RIGHT_SHIFT;}

";"		{return token::SEMI;}
":"		{return token::COLON;}
","		{return token::COMA;}
"."		{return token::DOT;}
".."		{return token::DOT;}

"="		{return token::EQUAL;}

"+"		{return token::PLUS;}
"-"		{return token::MINUS;}
"*"		{return token::MULTIPLY;}
"/"		{return token::DIVIDE;}
"%"		{return token::MODULO;}
"|"		{return token::OR;}
"^"		{return token::XOR;}
"&"		{return token::AND;}
"~"		{return token::NEGATE;}

"<"		{return token::LESS;}
">"		{return token::GREATER;}
"("		{return token::LEFT;}
")"		{return token::RIGHT;}
"{"		{return token::LEFT_CURLY_BRACKET;}
"}"		{return token::RIGHT_CURLY_BRACKET;}
"["		{return token::LEFT_SQUARE_BRACKET;}
"]"		{return token::RIGHT_SQUARE_BRACKET;}



%%

#ifdef yylex
#undef yylex
#endif

namespace idl {

Scanner::Scanner(std::istream* in,
		 std::ostream* out)
    : yyFlexLexer(in, out)
{
}

Scanner::~Scanner()
{
}
}

ParserDriver::ParserDriver()
    : m_top_scope(ScopeType::Top, "")
{}

ParserDriver::~ParserDriver()
{
}

bool
ParserDriver::parse(std::istream &in)
{
    m_scanner.reset(new idl::Scanner(&in));

    idl::Parser parser(*this);
//    parser.set_debug_level(10);
    return (parser.parse() == 0);
}

idl::Scanner &
ParserDriver::get_scanner()
{
    return *m_scanner.get();
}

int yyFlexLexer::yylex()
{
    std::cerr << "in ExampleFlexLexer::yylex() !" << std::endl;
    return 0;
}
