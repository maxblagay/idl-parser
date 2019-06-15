#include "scanner.hpp"
#include "Parser.hpp"

#include <assert.h>

namespace ast
{
ScopeDef *
scope(Node *node)
{
    if (!node)
    {
        return nullptr;
    }

    //    auto name = node->as<Name>();

    auto seq = node->as<ScopeDef>();
    assert(seq);
    return seq;
}

ScopeDef *
make_scope()
{
    return new ScopeDef(ScopeType::NAV, "");
}

ScopeDef *
make_scope(Node *node)
{
    auto seq = make_scope();
    if (node)
    {
        seq->push_back(node);
    }

    return seq;
}

ScopeDef *
make_scope(ScopeType scope_type, Node *node)
{
    assert(node);

    auto name = node->as<Name>();
    return new ScopeDef(scope_type, name->get_name());
}

InterfaceDef *
intf(Node *node)
{
    if (!node)
    {
        return nullptr;
    }

    auto seq = node->as<InterfaceDef>();
    assert(seq);
    return seq;
}

InterfaceDef *
make_intf(Node *node)
{
    assert(node);

    auto name = node->as<Name>();
    return new InterfaceDef(name->get_name());
}

} // namespace ast
