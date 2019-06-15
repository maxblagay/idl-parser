#pragma once

#include "IdlType.hpp"

namespace ast
{
struct Node
{
    Node(const Node &) = delete;

    Node(Type node_type);

    template<typename T>
    T *as()
    {
        if (T::type() != m_node_type)
        {
            return nullptr;
        }

        return static_cast<T *>(this);
    }

    auto get_type() const
    {
        return m_node_type;
    }

public:
    const Type m_node_type = Type::NAV;
};

} // namespace ast
