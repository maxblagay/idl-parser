#include "types.hpp"

#include <map>

namespace ast
{
std::string
to_string(Type type)
{
    static const std::map<Type, std::string> s_values = {};

    auto it = s_values.find(type);
    if (it == s_values.end())
    {
        return std::to_string((int)type);
    }

    return it->second;
}

std::string
to_string(ScopeType scope_type)
{
    static const std::map<ScopeType, std::string> s_values = {
        {ScopeType::Top, "file"},
        {ScopeType::Module, "module"},
        {ScopeType::Interface, "interface"},
        {ScopeType::Struct, "struct"},
        {ScopeType::Union, "union"},
        {ScopeType::Enum, "enum"},

    };

    auto it = s_values.find(scope_type);
    if (it == s_values.end())
    {
        return std::to_string((int)scope_type);
    }

    return it->second;
}

std::string
to_string(InterfaceScope scope)
{
    static const std::map<InterfaceScope, std::string> s_values = {
        {InterfaceScope::Abstract, "abstract"},
        {InterfaceScope::Local, "local"},

    };

    auto it = s_values.find(scope);
    if (it == s_values.end())
    {
        return std::to_string((int)scope);
    }

    return it->second;
}

std::string
to_string(ParamDirection direction)
{
    static const std::map<ParamDirection, std::string> s_values = {
        {ParamDirection::In, "in"},
        {ParamDirection::Out, "out"},
        {ParamDirection::InOut, "inout"},

    };

    auto it = s_values.find(direction);
    if (it == s_values.end())
    {
        return std::to_string((int)direction);
    }

    return it->second;
}

std::string
to_string(OpType direction)
{
    static const std::map<OpType, std::string> s_values = {{OpType::Or, "|"},
                                                           {OpType::Xor, "^"},
                                                           {OpType::And, "&"},
                                                           {OpType::ShiftLeft, "<<"},
                                                           {OpType::ShiftRight, ">>"},
                                                           {OpType::Plus, "+"},
                                                           {OpType::Minus, "-"},
                                                           {OpType::Mul, "*"},
                                                           {OpType::Div, "/"},
                                                           {OpType::Mod, "%"},
                                                           {OpType::Not, "~"}};

    auto it = s_values.find(direction);
    if (it == s_values.end())
    {
        return std::to_string((int)direction);
    }

    return it->second;
}
} // namespace ast
