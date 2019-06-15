#pragma once

#include <cstdint>
#include <string>

namespace ast
{
enum class Type : uint8_t
{
    NAV,

    NodeType,
    Name,

    OneWay,
    Abstract,
    Local,
    Fixed,
    ValueBased,

    TypeDef,
    SequenceDef,

    VariableDef,
    ArrayDef,

    ForwardDecl,
    InterfaceDef,
    AttributeDef,

    OperationDef,
    ParamAttr,
    ParamDef,

    ScopeDef,
    MemberDef,
    UnionDef,
    CaseDef,
    ExceptionDef,

    BinaryOpDef,
    UnaryOpDef,

    Literal

};

std::string to_string(Type type);

enum class ScopeType : uint8_t
{
    Top,
    Module,
    Interface,
    Struct,
    Union,
    Enum,
    Exception,

    NAV
};

std::string to_string(ScopeType scope_type);

enum class InterfaceScope : uint8_t
{
    Undef,
    Abstract,
    Local
};

std::string to_string(InterfaceScope scope);

enum class ParamDirection : uint8_t
{
    In,
    Out,
    InOut,

    NAV
};

std::string to_string(ParamDirection direction);

enum class OpType : uint8_t
{
    NAV,

    Or,
    Xor,
    And,
    ShiftLeft,
    ShiftRight,
    Plus,
    Minus,
    Mul,
    Div,
    Mod,
    Not,
};

std::string to_string(OpType direction);

} // namespace ast
