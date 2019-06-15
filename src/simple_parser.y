%require "3.0"
%language "c++"

%defines
%skeleton "lalr1.cc"

%define api.prefix {idl}
%define api.parser.class {Parser}


%param { ParserDriver& driver }

%locations

%define parse.error verbose

%token END 0	"end of file"
%token SEMI COMA COLON DOT
%token UNSIGNED
%token SHORT LONG FLOAT DOUBLE CHAR WCHAR BOOLEAN ANY OCTET OBJECT VOID STRING WSTRING
%token INTEGER NAME
%token MODULE INTERFACE STRUCT UNION ENUM TYPEDEF SEQUENCE SWITCH CASE DEFAULT EXCEPTION
%token READONLY ATTRIBUTE
%token ABSTRACT LOCAL
%token CONST
%token IN OUT INOUT ONEWAY
%token PLUS MINUS MULTIPLY DIVIDE MODULO NEGATE OR XOR AND
%token LEFT RIGHT LEFT_CURLY_BRACKET RIGHT_CURLY_BRACKET LEFT_SQUARE_BRACKET RIGHT_SQUARE_BRACKET
%token LESS GREATER EQUAL
%token SCOPE
%token LEFT_SHIFT RIGHT_SHIFT
%token TRUE FALSE
%token FIXED
%token VALUEBASED
%token NATIVE
%token RAISES
%token QUIT

%{

#include <assert.h>
#include "helpers.hpp"
#include "scanner.hpp"

using namespace ast;

#undef yylex
#define yylex driver.get_scanner().lex

%}

%glr-parser

%start specification

%%

// 1
specification:
    definitions
    {
        auto &seq = driver.get_top_scope();
        seq.extend(scope($1));
    }
;

definitions:
    %empty
    {
        $$ = make_scope();
    }
    | definition
    {
        auto scope = make_scope();
        scope->extend($1);
        $$ = scope;
    }
    | definitions definition
    {
        assert($1);
        scope($1)->extend($2);
    }
;

// 2
definition:
    type_dcl SEMI
    | const_dcl SEMI
    | except_dcl SEMI
    | interface SEMI
    | module SEMI
    | value SEMI
;

// 3
module:
    MODULE identifier LEFT_CURLY_BRACKET definitions RIGHT_CURLY_BRACKET
    {
        auto seq = make_scope(ScopeType::Module, $2);
        seq->extend($4);

        $$ = seq;
    }
;

// 4
interface:
    interface_dcl
    | forward_dcl
;

// 5
interface_dcl:
    interface_header LEFT_CURLY_BRACKET interface_body RIGHT_CURLY_BRACKET
    {
        auto seq = intf($1);
        seq->extend($3);

        $$ = seq;
    }
;

// 6
forward_dcl:
    interface_type INTERFACE identifier
    {
        auto name = $3->as<Name>();
        assert(name);

        auto seq = new ForwardDecl(ScopeType::Interface, name->get_name());
        seq->set_scope($1);

        $$ = seq;
    }
;

// 7
interface_header:
    interface_type INTERFACE identifier interface_inheritance_spec
    {
        auto seq = make_intf($3);
        seq->set_scope($1);
        seq->set_super_types($4);

        $$ = seq;
    }
;

interface_type:
    %empty
    { $$ = nullptr; }
    | ABSTRACT
    | LOCAL
;

// 8
interface_body:
    %empty
    {
        $$ = make_scope();
    }
    | export
    {
        auto scope = make_scope();
        scope->extend($1);

        $$ = scope;
    }
    | interface_body export
    {
        scope($1)->extend($2);
    }
;

// 9
export:
    type_dcl SEMI
    | const_dcl SEMI
    | except_dcl SEMI
    | attr_dcl SEMI
    | op_dcl SEMI
;

// 10
interface_inheritance_spec:
    %empty
    { $$ = nullptr; }
    | COLON supers
    { $$ = $2; }
;

supers:
    interface_name
    {
        auto scope = make_scope();
        scope->extend($1);

        $$ = scope;
    }
    | supers COMA interface_name
    {
        scope($1)->push_back($3);
    }
;

// 11
interface_name:
    scoped_name
;

// 12
scoped_name:
    identifier
    {
        auto scope = make_scope();
        scope->push_back($1);

        $$ = scope;
    }
    | SCOPE identifier
    {
        auto scope = make_scope();
        scope->push_back($2);

        $$ = scope;
    }
    | scoped_name SCOPE identifier
    {
        scope($1)->push_back($3);
    }
;

// 13
value: %empty
;

// 27
const_dcl:
    CONST const_type identifier EQUAL const_exp
    {
        auto name = $3->as<Name>();
        assert(name);

        $$ = new VariableDef($2, name->get_name(), $5);
    }
;

// 28
const_type:
    integer_type
    | char_type
    | wide_char_type
    | boolean_type
    | floating_pt_type
    | string_type
    | wide_string_type
    | fixed_pt_const_type
    | scoped_name
    | octet_type
;

// 29
const_exp:
    or_expr
;

// 30
or_expr:
    xor_expr
    | or_expr OR xor_expr
    {
        $$ = new Binary(OpType::Or, $1, $3);
    }
;

// 31
xor_expr:
    and_expr
    | xor_expr XOR and_expr
    {
        $$ = new Binary(OpType::Xor, $1, $3);
    }
;

// 32
and_expr:
    shift_expr
    | and_expr AND shift_expr
    {
        $$ = new Binary(OpType::And, $1, $3);
    }
;

// 33
shift_expr:
    add_expr
    | shift_expr RIGHT_SHIFT add_expr
    {
        $$ = new Binary(OpType::ShiftRight, $1, $3);
    }
    | shift_expr LEFT_SHIFT add_expr
    {
        $$ = new Binary(OpType::ShiftLeft, $1, $3);
    }
;

// 34
add_expr:
    mult_expr
    | add_expr PLUS mult_expr
    {
        $$ = new Binary(OpType::Plus, $1, $3);
    }
    | add_expr MINUS mult_expr
    {
        $$ = new Binary(OpType::Minus, $1, $3);
    }
;

// 35
mult_expr:
    unary_expr
    | mult_expr MULTIPLY unary_expr
    {
        $$ = new Binary(OpType::Mul, $1, $3);
    }
    | mult_expr DIVIDE unary_expr
    {
        $$ = new Binary(OpType::Div, $1, $3);
    }
    | mult_expr MODULO unary_expr
    {
        $$ = new Binary(OpType::Mod, $1, $3);
    }
;

// 36
unary_expr:
    unary_operator primary_expr
    {
        auto unary = $1->as<Unary>();
        assert(unary);

        $$ = new Unary(unary->get_op_type(), $2);
    }
    | primary_expr
;

// 37
unary_operator:
    MINUS
    {
        $$ = new Unary(OpType::Minus, nullptr);
    }
    | PLUS
    {
        $$ = new Unary(OpType::Plus, nullptr);
    }
    | NEGATE
    {
        $$ = new Unary(OpType::Not, nullptr);
    }
;

// 38
primary_expr:
    scoped_name
    | literal
    | LEFT const_exp RIGHT
    {
        $$ = new Unary(OpType::NAV, $2);
    }
;

// 39
literal:
    integer_literal
    | boolean_literal
;

// 40
boolean_literal:
    TRUE
    | FALSE
;

// 41
positive_int_const:
    const_exp
;

// 42
type_dcl:
    TYPEDEF type_declarator
    { $$ = $2; }
    | struct_type
    | union_type
    | enum_type
    | NATIVE simple_declarator
    { $$ = $2; }
    | constr_forward_decl
;

// 43
type_declarator:
    type_spec declarators
    {
        $$ = TypeDef::make($1, $2);
    }
;

// 44
type_spec:
    simple_type_spec
    | constr_type_spec
;

// 45
simple_type_spec:
    base_type_spec
    | template_type_spec
    | scoped_name
;

// 46
base_type_spec:
    floating_pt_type
    | integer_type
    | char_type
    | wide_char_type
    | boolean_type
    | octet_type
    | any_type
    | object_type
    | value_base_type
;

// 47
template_type_spec:
    sequence_type
    | string_type
    | wide_string_type
    | fixed_pt_type
;

// 48
constr_type_spec:
    struct_type
    | union_type
    | enum_type
;

// 49
declarators:
    declarator
    {
        auto scope = make_scope();
        scope->extend($1);

        $$ = scope;
    }
    | declarators COMA declarator
    {
        scope($1)->push_back($3);
    }
;

// 50
declarator:
    simple_declarator
    | complex_declarator
;

// 51
simple_declarator:
    identifier
;

simple_declarators:
    simple_declarator
    {
        auto scope = make_scope();
        scope->push_back($1);

        $$ = scope;
    }
    | simple_declarators COMA simple_declarator
    {
        scope($1)->push_back($3);
    }
;

// 52
complex_declarator:
    array_declarator
;

// 53
floating_pt_type:
    FLOAT
    | DOUBLE
    | LONG DOUBLE
    {
        $$ = new NodeType(IdlType::LongDouble);
    }
;

// 54
integer_type:
    signed_int
    | unsigned_int
;

// 55
signed_int:
    signed_short_int
    | signed_long_int
    | signed_longlong_int
;

// 56
signed_short_int:
    SHORT
;

// 57
signed_long_int:
    LONG
;

// 58
signed_longlong_int:
    LONG LONG
    {
        $$ = new NodeType(IdlType::LongLong);
    }
;

// 59
unsigned_int:
    unsigned_short_int
    | unsigned_long_int
    | unsigned_longlong_int
;

// 60
unsigned_short_int:
    UNSIGNED SHORT
    {
        $$ = new NodeType(IdlType::UnsignedShort);
    }
;

// 61
unsigned_long_int:
    UNSIGNED LONG
    {
        $$ = new NodeType(IdlType::UnsignedLong);
    }
;

// 62
unsigned_longlong_int:
    UNSIGNED LONG LONG
    {
        $$ = new NodeType(IdlType::UnsignedLongLong);
    }
;

// 63
char_type:
    CHAR
;

// 64
wide_char_type:
    WCHAR
;

// 65
boolean_type:
    BOOLEAN
;

// 66
octet_type:
    OCTET
;

// 67
any_type:
    ANY
;

// 68
object_type:
    OBJECT
;

// 69
struct_type: STRUCT identifier LEFT_CURLY_BRACKET member_list RIGHT_CURLY_BRACKET
    {
        auto seq = make_scope(ScopeType::Struct, $2);
        seq->extend($4);

        $$ = seq;
    }
;

// 70
member_list:
    %empty
    { $$ = nullptr; }
    | member
    {
        auto scope = make_scope();
        scope->extend($1);

        $$ = scope;
    }
    | member_list member
    {
        scope($1)->extend($2);
    }
;

// 71
member:
    type_spec declarators SEMI
    {
        $$ = MemberDef::make($1, $2);
    }
;

// 72
union_type:
    UNION identifier SWITCH LEFT switch_type_spec RIGHT LEFT_CURLY_BRACKET switch_body RIGHT_CURLY_BRACKET
    {
        auto name = $2->as<Name>();
        assert(name);

        auto union_type = $4->as<Name>();
        assert(union_type);

        auto uni = new UnionDef(name->get_name(), union_type->get_name());
        uni->extend($8);

        $$ = uni;
    }
;

// 73
switch_type_spec:
    integer_type
    | char_type
    | boolean_type
    | enum_type
    | scoped_name
;

// 74
switch_body:
    case
    {
        auto scope = make_scope();
        scope->extend($1);

        $$ = scope;
    }
    | switch_body case
    {
        scope($1)->push_back($2);
    }
;

// 75
case:
    case_labels element_spec SEMI
    {
        auto element = $2->as<CaseDef>();
        assert(element);

        element->set_labels($1);

        $$ = element;
    }
;

case_labels:
    case_label
    {
        auto scope = make_scope();
        scope->extend($1);

        $$ = scope;
    }
    | case_labels case_label
    {
        scope($1)->push_back($2);
    }
;

// 76
case_label:
    CASE const_exp COLON
    { $$ = $2; }
    | DEFAULT COLON
    {
        $$ = new Name("default");
    }
;

// 77
element_spec:
    type_spec declarator
    {
        auto name = $2->as<Name>();
        assert(name);

        $$ = new CaseDef(name->get_name(), get_type_value($2));
    }
;

// 78
enum_type:
    ENUM identifier LEFT_CURLY_BRACKET enumerators RIGHT_CURLY_BRACKET
    {
        auto seq = make_scope(ScopeType::Enum, $2);
        seq->extend($4);

        $$ = seq;
    }
;

enumerators:
    enumerator
    {
        auto scope = make_scope();
        scope->push_back($1);

        $$ = scope;
    }
    | enumerators COMA enumerator
    {
        scope($1)->push_back($3);
    }
;

// 79
enumerator:
    identifier
;

// 80
sequence_type:
    SEQUENCE LESS simple_type_spec COMA positive_int_const GREATER
    {
        $$ = new SequenceDef($3, $5);
    }
    | SEQUENCE LESS simple_type_spec GREATER
    {
        $$ = new SequenceDef($3);
    }
;

// 81
string_type:
    STRING LESS positive_int_const GREATER
    {
        $$ = new NodeType("string", $3);
    }
    | STRING
;

// 82
wide_string_type:
    WSTRING LESS positive_int_const GREATER
    {
        $$ = new NodeType("wstring", $3);
    }
    | WSTRING
;

// 83
array_declarator:
    identifier fixed_array_size
    {
        auto name = $1->as<Name>();
        assert(name);

        $$ = new ArrayDef(name->get_name(), $2);
    }
;

// 84
fixed_array_size:
    LEFT_SQUARE_BRACKET positive_int_const RIGHT_SQUARE_BRACKET
    { $$ = $2; }
;

// 85
attr_dcl:
    READONLY ATTRIBUTE param_type_spec simple_declarators
    {
        $$ = AttributeDef::make($3, $4, true);
    }
    | ATTRIBUTE param_type_spec simple_declarators
    {
        $$ = AttributeDef::make($2, $3);
    }
;

// 86
except_dcl:
    EXCEPTION identifier LEFT_CURLY_BRACKET member_list RIGHT_CURLY_BRACKET
    {
        auto name = $2->as<Name>();
        assert(name);

        auto excp = new ExceptionDef(name->get_name());
        excp->extend($4);

        $$ = excp;
    }
;

// 87
op_dcl: op_attribute op_type_spec identifier parameter_dcls raises_expr context_expr
    {
        auto name = $3->as<Name>();
        assert(name);

        auto op = new OperationDef($2, name->get_name());
        op->set_params($4);

        if ($1 && $1->get_type() == Type::OneWay)
        {
            op->set_oneway(true);
        }

        op->set_raises($5);
        op->set_context($6);

        $$ = op;
    }
;

// 88
op_attribute:
    %empty
    { $$ = nullptr; }
    | ONEWAY
    { $$ = $1; }
;

// 89
op_type_spec:
    param_type_spec
    | VOID
;

// 90
parameter_dcls: LEFT param_dcls RIGHT
    { $$ = $2; }
;

param_dcls:
    %empty
    { $$ = nullptr; }
    | param_dcl
    {
        auto scope = make_scope();
        scope->push_back($1);

        $$ = scope;
    }
    | param_dcls COMA param_dcl
    {
        scope($1)->push_back($3);
    }
;

// 91
param_dcl:
    param_attribute param_type_spec simple_declarator
    {
        $$ = ParamDef::make($1, $2, $3);
    }
;

// 92
param_attribute:
    IN
    | OUT
    | INOUT
;

// 93
raises_expr:
    %empty
    { $$ = nullptr; }
    | RAISES LEFT exception_list RIGHT
    { $$ = $3; }
;

exception_list:
    scoped_name
    {
        auto scope = make_scope();
        scope->push_back($1);

        $$ = scope;
    }
    | exception_list COMA scoped_name
    {
        scope($1)->push_back($3);
    }
;

// 94
context_expr:
    %empty
;

// 95
param_type_spec:
    base_type_spec
    | NAME
;

// 96
fixed_pt_type:
    FIXED LESS positive_int_const COMA positive_int_const GREATER
    {
        $$ = new NodeType($3, $5);
    }
;

// 97
fixed_pt_const_type:
    FIXED
;

// 98
value_base_type:
    VALUEBASED
;

// 99
constr_forward_decl:
    STRUCT identifier
    { $$ = $2; }
    | UNION identifier
    { $$ = $2; }
;

identifier: NAME
    { $$ = $1; }
;

integer_literal: INTEGER
    { $$ = $1; }
;


%%

namespace idl
{
void
Parser::error( const Parser::location_type& l, const std::string& m)
{
    driver.parse_error(l.begin.line, l.begin.column, m);
}

}

