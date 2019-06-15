#pragma once

#include "Parser.hpp"

namespace ast
{
template<typename CALLBACK>
Node *
multi_make(Node *names, CALLBACK &&on_node)
{
    if (auto seq = names->as<ScopeDef>())
    {
        auto &children = seq->get_children();
        if (children.size() == 1)
        {
            auto value = on_node(children.front());
            return value;
        }
        else
        {
            auto attrs = new ScopeDef(ScopeType::NAV, "");
            for (auto child : children)
            {
                auto value = on_node(child);
                attrs->push_back(value);
            }

            return attrs;
        }
    }

    auto value = on_node(names);
    return value;
}

template<typename CALLBACK>
void
process(Node *nodes, CALLBACK &&on_node)
{
    if (auto seq = nodes->as<ScopeDef>())
    {
        auto &children = seq->get_children();
        for (auto child : children)
        {
            process(child, on_node);
        }
    }
    else
    {
        on_node(nodes);
    }
}

std::string get_type_value(Node *type_node);

ScopeDef *scope(Node *node);

ScopeDef *make_scope();

ScopeDef *make_scope(ScopeType scope_type, Node *node);

InterfaceDef *intf(Node *node);

InterfaceDef *make_intf(Node *node);

} // namespace ast
