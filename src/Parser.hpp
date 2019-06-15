#pragma once

#include "Node.hpp"

#include <vector>

namespace ast
{
struct NodeType : Node
{
    NodeType(IdlType type);

    NodeType(const std::string &value);

    NodeType(const std::string &value, Node *first);

    NodeType(Node *first, Node *second);

    constexpr static Type type()
    {
        return Type::NodeType;
    }

    const auto &get_value() const
    {
        return m_value;
    }

private:
    std::string m_value;
    Node *m_first = nullptr;
    Node *m_second = nullptr;
};

struct NodeSequence : public Node
{
    NodeSequence(Type type);

    void push_back(Node *child);

    void extend(Node *other);

    auto size() const
    {
        return m_children.size();
    }

    const auto &get_children() const
    {
        return m_children;
    }

private:
    std::vector<Node *> m_children;
};

struct Named
{
    Named(const std::string &value);

    const auto &get_name() const
    {
        return m_value;
    }

private:
    std::string m_value;
};

struct Name : public Node, Named
{
    Name(const std::string &value);

    constexpr static Type type()
    {
        return Type::Name;
    }

private:
    Type m_type;
};

struct VariableDef : public Node, Named
{
    VariableDef(Node *type, const std::string &name, Node *value);

    constexpr static Type type()
    {
        return Type::VariableDef;
    }

    auto get_type() const
    {
        return m_type;
    }

    auto get_value() const
    {
        return m_value;
    }

private:
    Node *m_type;
    Node *m_value;
};

struct ForwardDecl : public Node, Named
{
    ForwardDecl(ScopeType scope_type, const std::string &name);

    constexpr static Type type()
    {
        return Type::ForwardDecl;
    }

    auto get_scope_type() const
    {
        return m_scope_type;
    }

    auto get_scope() const
    {
        return m_scope;
    }

    void set_scope(InterfaceScope value);

    void set_scope(Node *node);

private:
    ScopeType m_scope_type;
    InterfaceScope m_scope = InterfaceScope::Undef;
};

struct ScopeDef : public NodeSequence, Named
{
    ScopeDef(ScopeType scope_type, const std::string &name);

    constexpr static Type type()
    {
        return Type::ScopeDef;
    }

    auto get_scope_type() const
    {
        return m_scope_type;
    }

protected:
    ScopeDef(Type type, ScopeType scope_type, const std::string &name);

private:
    ScopeType m_scope_type;
};

struct InterfaceDef : public NodeSequence, Named
{
    InterfaceDef(const std::string &name);

    constexpr static Type type()
    {
        return Type::InterfaceDef;
    }

    auto get_scope() const
    {
        return m_scope;
    }

    void set_scope(Node *node);

    const auto &get_super_types() const
    {
        return m_super_types;
    }

    void set_super_types(Node *node);

private:
    InterfaceScope m_scope = InterfaceScope::Undef;
    std::vector<std::vector<std::string>> m_super_types;
};

struct AttributeDef : public Node, Named
{
    AttributeDef(Node *attr_type, const std::string &name);

    constexpr static Type type()
    {
        return Type::AttributeDef;
    }

    const auto &get_attr_type() const
    {
        return m_attr_type;
    }

    void set_readonly(bool value);

    static Node *make(Node *attr_type, Node *names, bool read_only = false);

private:
    std::string m_attr_type;
    bool m_readonly = false;
};

struct MemberDef : public Node, Named
{
    MemberDef(Node *member_type, const std::string &name);

    constexpr static Type type()
    {
        return Type::MemberDef;
    }

    const auto &get_member_type() const
    {
        return m_member_type;
    }

    auto get_array_dimension() const
    {
        return m_array_dimention;
    }

    void set_array_dimension(Node *dimension);

    static Node *make(Node *member_type, Node *names);

private:
    std::string m_member_type;
    Node *m_array_dimention = nullptr;
};

struct ParamAttr : public Node
{
    ParamAttr(ParamDirection value);

    auto get_direction() const
    {
        return m_value;
    }

    constexpr static Type type()
    {
        return Type::ParamAttr;
    }

private:
    ParamDirection m_value;
};

struct ParamDef : public Node, Named
{
    ParamDef(ParamDirection direction, Node *param_type, const std::string &name);

    constexpr static Type type()
    {
        return Type::ParamDef;
    }

    auto get_direction() const
    {
        return m_direction;
    }

    const auto &get_param_type() const
    {
        return m_param_type;
    }

    static ParamDef *make(Node *direction, Node *param_type, Node *name);

private:
    ParamDirection m_direction;
    std::string m_param_type;
};

struct OperationDef : public Node, Named
{
    OperationDef(Node *return_type, const std::string &name);

    constexpr static Type type()
    {
        return Type::OperationDef;
    }

    const auto &get_return_type() const
    {
        return m_return_type;
    }

    const auto &get_params() const
    {
        return m_params;
    }

    const auto &get_raises() const
    {
        return m_raises;
    }

    auto is_oneway() const
    {
        return m_is_one_way;
    }

    void set_params(Node *params);

    void set_oneway(bool value);

    void set_raises(Node *node);

    void set_context(Node *node);

private:
    std::string m_return_type;

    std::vector<ParamDef *> m_params;

    std::vector<std::vector<std::string>> m_raises;

    bool m_is_one_way = false;
};

struct SequenceDef : Node
{
    SequenceDef(Node *sequence_type, Node *size = nullptr);

    constexpr static Type type()
    {
        return Type::SequenceDef;
    }

    auto get_sequence_type() const
    {
        return m_sequence_type;
    }

    auto get_sequence_size() const
    {
        return m_sequence_size;
    }

private:
    Node *m_sequence_type;
    Node *m_sequence_size;
};

struct TypeDef : public Node, Named
{
    TypeDef(Node *type_def, const std::string &name);

    constexpr static Type type()
    {
        return Type::TypeDef;
    }

    auto get_typedef() const
    {
        return m_typedef;
    }

    static Node *make(Node *type_def, Node *names);

private:
    Node *m_typedef;
};

struct ArrayDef : public Node, Named
{
    ArrayDef(const std::string &name, Node *dimension);

    constexpr static Type type()
    {
        return Type::ArrayDef;
    }

    Node *get_dimension() const
    {
        return m_dimension;
    }

private:
    Node *m_dimension;
};

struct UnionDef : public ScopeDef
{
    UnionDef(const std::string &name, const std::string &union_type);

    constexpr static Type type()
    {
        return Type::UnionDef;
    }

    const auto &get_union_type() const
    {
        return m_union_type;
    }

private:
    std::string m_union_type;
};

struct CaseDef : public Node, Named
{
    CaseDef(const std::string &name, const std::string &case_type);

    constexpr static Type type()
    {
        return Type::CaseDef;
    }

    const auto &get_case_type() const
    {
        return m_case_type;
    }

    const auto &get_labels() const
    {
        return m_labels;
    }

    void set_labels(Node *node);

private:
    std::vector<std::string> m_labels;
    std::string m_case_type;
};

struct ExceptionDef : public ScopeDef
{
    ExceptionDef(const std::string &name);

    constexpr static Type type()
    {
        return Type::ExceptionDef;
    }
};

struct Binary : public Node
{
    Binary(OpType op_type, Node *a, Node *b);

    constexpr static Type type()
    {
        return Type::BinaryOpDef;
    }

    auto get_op_type() const
    {
        return m_op_type;
    }

    auto get_left()
    {
        return m_left;
    }

    auto get_right()
    {
        return m_right;
    }

private:
    OpType m_op_type;
    Node *m_left;
    Node *m_right;
};

struct Unary : public Node
{
    Unary(OpType op_type, Node *a);

    constexpr static Type type()
    {
        return Type::UnaryOpDef;
    }

    auto get_op_type() const
    {
        return m_op_type;
    }

    auto get_left()
    {
        return m_left;
    }

private:
    OpType m_op_type;
    Node *m_left;
};

struct Literal : public Node
{
    Literal(const std::string &value);

    constexpr static Type type()
    {
        return Type::Literal;
    }

    const auto &get_value() const
    {
        return m_value;
    }

private:
    std::string m_value;
};

} // namespace ast
