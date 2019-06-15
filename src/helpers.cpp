#include "helpers.hpp"

#include <assert.h>

namespace ast
{
std::string
get_type_value(Node *type_node)
{
    if (auto node_type = type_node->as<NodeType>())
    {
        return node_type->get_value();
    }
    else if (auto node_type = type_node->as<Name>())
    {
        return node_type->get_name();
    }
    else
    {
        assert(false && "Not implemented");
    }

    return "";
}

} // namespace ast
