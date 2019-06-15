#pragma once

#include <fstream>
#include <string_view>

namespace ast
{
struct Node;
}

namespace utils
{
bool init(const std::string &path, const std::string_view &data);

void indent(std::ostream &out, int value);

void print_nodes(std::ostream &out, ast::Node *node, int indent = 0);

} // namespace utils
