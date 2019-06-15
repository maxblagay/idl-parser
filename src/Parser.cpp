#include "Parser.hpp"
#include "helpers.hpp"

#include <assert.h>
#include <iostream>
#include <map>

namespace ast
{
std::string
to_string(IdlType type)
{
    static const std::map<IdlType, std::string> s_values = {
        {IdlType::Short, "short"},
        {IdlType::Long, "long"},
        {IdlType::LongLong, "long long"},

        {IdlType::UnsignedShort, "unsigned short"},
        {IdlType::UnsignedLong, "unsigned long"},
        {IdlType::UnsignedLongLong, "unsigned long long"},

        {IdlType::Float, "float"},
        {IdlType::Double, "double"},
        {IdlType::LongDouble, "long double"},

        {IdlType::Char, "char"},
        {IdlType::WChar, "wchar"},

        {IdlType::String, "string"},
        {IdlType::WString, "wstring"},

        {IdlType::Boolean, "boolean"},
        {IdlType::Any, "any"},
        {IdlType::Octet, "octet"},

        {IdlType::Void, "void"},

        {IdlType::Object, "Object"},

    };

    auto it = s_values.find(type);
    if (it == s_values.end())
    {
        return std::to_string((int)type);
    }

    return it->second;
}

Node::Node(Type node_type)
    : m_node_type(node_type)
{
}

NodeType::NodeType(IdlType idl_type)
    : Node(type())
    , m_value(to_string(idl_type))
{
}

NodeType::NodeType(const std::string &value)
    : Node(type())
    , m_value(value)
{
}

NodeType::NodeType(const std::string &value, Node *first)
    : Node(type())
    , m_value(value)
    , m_first(first)
{
}

NodeType::NodeType(Node *first, Node *second)
    : Node(type())
    , m_first(first)
    , m_second(second)
{
    m_value = "fixed";
}

NodeSequence::NodeSequence(Type type)
    : Node(type)
{
}

void
NodeSequence::push_back(Node *child)
{
    m_children.push_back(child);
}

void
NodeSequence::extend(Node *other)
{
    if (!other)
    {
        return;
    }

    if (auto seq = other->as<ScopeDef>())
    {
        if (seq->get_scope_type() == ScopeType::NAV)
        {
            auto &other_children = seq->get_children();
            m_children.insert(m_children.end(), other_children.begin(), other_children.end());
        }
        else
        {
            m_children.push_back(other);
        }
    }
    else
    {
        m_children.push_back(other);
    }
}

Binary::Binary(OpType op_type, Node *a, Node *b)
    : Node(type())
    , m_op_type(op_type)
    , m_left(a)
    , m_right(b)
{
}

Named::Named(const std::string &value)
    : m_value(value)
{
}

Name::Name(const std::string &value)
    : Node(type())
    , Named(value)
{
}

VariableDef::VariableDef(Node *variable_type, const std::string &name, Node *value)
    : Node(type())
    , Named(name)
    , m_type(variable_type)
    , m_value(value)
{
}

ForwardDecl::ForwardDecl(ScopeType scope_type, const std::string &name)
    : Node(type())
    , Named(name)
    , m_scope_type(scope_type)
{
}

void
ForwardDecl::set_scope(InterfaceScope value)
{
    m_scope = value;
}

void
ForwardDecl::set_scope(Node *node)
{
    if (!node)
    {
        return;
    }

    switch (node->get_type())
    {
    case Type::Abstract:
        set_scope(InterfaceScope::Abstract);
        break;

    case Type::Local:
        set_scope(InterfaceScope::Local);
        break;

    default:
        assert(false && "Unknown interface scope");
        break;
    }
}

ScopeDef::ScopeDef(ScopeType scope_type, const std::string &name)
    : ScopeDef(type(), scope_type, name)
{
}

ScopeDef::ScopeDef(Type node_type, ScopeType scope_type, const std::string &name)
    : NodeSequence(node_type)
    , Named(name)
    , m_scope_type(scope_type)
{
}

InterfaceDef::InterfaceDef(const std::string &name)
    : NodeSequence(type())
    , Named(name)
{
}

void
InterfaceDef::set_scope(Node *node)
{
    if (!node)
    {
        return;
    }

    switch (node->get_type())
    {
    case Type::Abstract:
        m_scope = InterfaceScope::Abstract;
        break;

    case Type::Local:
        m_scope = InterfaceScope::Local;
        break;

    default:
        assert(false && "Unknown interface scope");
        break;
    }
}

void
InterfaceDef::set_super_types(Node *node)
{
    if (!node)
    {
        return;
    }

    process(node, [&](Node *node) {
        if (auto super_name = node->as<Name>())
        {
            m_super_types.push_back({super_name->get_name()});
        }
        else
        {
            assert(false && "Not implemented");
        }
    });
}

AttributeDef::AttributeDef(Node *attr_type, const std::string &name)
    : Node(type())
    , Named(name)
    , m_attr_type(get_type_value(attr_type))
{
}

void
AttributeDef::set_readonly(bool value)
{
    m_readonly = value;
}

Node *
AttributeDef::make(Node *attr_type, Node *names, bool read_only)
{
    if (!attr_type || !names)
    {
        return nullptr;
    }

    return multi_make(names, [&](Node *name_node) {
        auto name = name_node->as<Name>();
        assert(name);

        auto attr = new AttributeDef(attr_type, name->get_name());
        attr->set_readonly(read_only);
        return attr;
    });
}

MemberDef::MemberDef(Node *member_type, const std::string &name)
    : Node(type())
    , Named(name)
    , m_member_type(get_type_value(member_type))
{
}

void
MemberDef::set_array_dimension(Node *dimension)
{
    m_array_dimention = dimension;
}

Node *
MemberDef::make(Node *member_type, Node *names)
{
    if (!member_type || !names)
    {
        return nullptr;
    }

    return multi_make(names, [&](Node *name_node) {
        if (auto name = name_node->as<Name>())
        {
            auto attr = new MemberDef(member_type, name->get_name());
            return attr;
        }
        else if (auto array_node = name_node->as<ArrayDef>())
        {
            auto attr = new MemberDef(member_type, array_node->get_name());
            attr->set_array_dimension(array_node->get_dimension());
            return attr;
        }
        else
        {
            assert(false && "Not implementd");
        }
    });
}

OperationDef::OperationDef(Node *return_type, const std::string &name)
    : Node(type())
    , Named(name)
    , m_return_type(get_type_value(return_type))
{
}

void
OperationDef::set_params(Node *params)
{
    if (!params)
    {
        return;
    }

    auto seq = params->as<ScopeDef>();
    assert(seq);

    for (auto param_node : seq->get_children())
    {
        auto param = param_node->as<ParamDef>();
        assert(param);

        m_params.push_back(param);
    }
}

void
OperationDef::set_oneway(bool value)
{
    m_is_one_way = value;
}

void
OperationDef::set_raises(Node *node)
{
    if (!node)
    {
        return;
    }

    if (auto seq = node->as<ScopeDef>())
    {
        for (const auto &excp_node : seq->get_children())
        {
            auto excp = excp_node->as<ScopeDef>();
            assert(excp);

            std::vector<std::string> name;
            for (const auto &part_node : excp->get_children())
            {
                auto part = part_node->as<Name>();
                assert(part);

                name.push_back(part->get_name());
            }

            m_raises.push_back(name);
        }
    }
    else
    {
        assert(false && "Not implemented");
    }
}

void
OperationDef::set_context(Node *node)
{
    if (!node)
    {
        return;
    }
}

ParamAttr::ParamAttr(ParamDirection value)
    : Node(type())
    , m_value(value)
{
}

ParamDef::ParamDef(ParamDirection direction, Node *param_type, const std::string &name)
    : Node(type())
    , Named(name)
    , m_direction(direction)
    , m_param_type(get_type_value(param_type))
{
}

ParamDef *
ParamDef::make(Node *direction, Node *param_type, Node *name)
{
    if (!direction || !param_type || !name)
    {
        return nullptr;
    }

    auto dir_node = direction->as<ParamAttr>();
    auto name_node = name->as<Name>();

    auto param = new ParamDef(dir_node->get_direction(), param_type, name_node->get_name());
    return param;
}

SequenceDef::SequenceDef(Node *sequence_type, Node *size)
    : Node(type())
    , m_sequence_type(sequence_type)
    , m_sequence_size(size)
{
}

TypeDef::TypeDef(Node *type_def, const std::string &name)
    : Node(type())
    , Named(name)
    , m_typedef(type_def)
{
}

Node *
TypeDef::make(Node *type_def, Node *names)
{
    return multi_make(names, [&](Node *name_node) {
        auto name = name_node->as<Name>();
        assert(name);

        auto value = new TypeDef(type_def, name->get_name());
        return value;
    });
}

ArrayDef::ArrayDef(const std::string &name, Node *dimension)
    : Node(type())
    , Named(name)
    , m_dimension(dimension)
{
    m_dimension = multi_make(dimension, [&](Node *name_node) { return name_node; });
}

Unary::Unary(OpType op_type, Node *a)
    : Node(type())
    , m_op_type(op_type)
    , m_left(a)
{
}

Literal::Literal(const std::string &value)
    : Node(type())
    , m_value(value)
{
}

UnionDef::UnionDef(const std::string &name, const std::string &union_type)
    : ScopeDef(Type::UnionDef, ScopeType::Union, name)
    , m_union_type(union_type)
{
}

CaseDef::CaseDef(const std::string &name, const std::string &case_type)
    : Node(type())
    , Named(name)
    , m_case_type(case_type)
{
}

void
CaseDef::set_labels(Node *node)
{
    if (auto seq = node->as<ScopeDef>())
    {
        for (const auto &label_node : seq->get_children())
        {
            if (auto name = label_node->as<Name>())
            {
                m_labels.push_back(name->get_name());
            }
            else if (auto name = label_node->as<Literal>())
            {
                m_labels.push_back(name->get_value());
            }
            else
            {
                assert(false && "Not implemented");
            }
        }
    }
    else
    {
        assert(false && "Not implemented");
    }
}

ExceptionDef::ExceptionDef(const std::string &name)
    : ScopeDef(Type::ExceptionDef, ScopeType::Exception, name)
{
}

} // namespace ast

