#include "TestUtils.hpp"

#include "Parser.hpp"

#include <assert.h>
#include <fstream>

namespace utils
{
bool
init(const std::string &path, const std::string_view &data)
{
    auto fp = fopen(path.c_str(), "wb+");
    if (!fp)
    {
        return false;
    }

    auto bytes = fwrite(data.data(), data.size(), 1, fp);
    if (!bytes)
    {
        return false;
    }

    fclose(fp);
    return true;
}

void
ind(std::ostream &out, int value)
{
    for (int i = 0; i < value; ++i)
    {
        out << "  ";
    }
}

void
print_nodes(std::ostream &out, ast::Node *node, int indent)
{
    if (auto scope = node->as<ast::ScopeDef>())
    {
        if (scope->get_scope_type() == ast::ScopeType::NAV)
        {
            for (const auto node : scope->get_children())
            {
                print_nodes(out, node);
            }

            return;
        }

        ind(out, indent);

        out << to_string(scope->get_scope_type()) << " " << scope->get_name() << " {" << std::endl;

        for (const auto node : scope->get_children())
        {
            print_nodes(out, node, indent + 1);
            out << std::endl;
        }

        ind(out, indent);
        out << "}" << std::endl;
    }
    else if (auto intf = node->as<ast::InterfaceDef>())
    {
        ind(out, indent);

        auto scope_type = intf->get_scope();
        if (scope_type != ast::InterfaceScope::Undef)
        {
            out << to_string(scope_type) << " ";
        }

        out << "interface " << intf->get_name();

        if (!intf->get_super_types().empty())
        {
            out << " : ";

            size_t index = 0;
            for (const auto &super : intf->get_super_types())
            {
                if (index > 0)
                {
                    out << ", ";
                }

                size_t name_index = 0;
                for (const auto &name_part : super)
                {
                    if (name_index > 0)
                    {
                        out << "::";
                    }

                    out << name_part;
                    ++name_index;
                }
                ++index;
            }
        }

        out << " {" << std::endl;

        for (const auto node : intf->get_children())
        {
            print_nodes(out, node, indent + 1);
            out << ";" << std::endl;
        }

        ind(out, indent);
        out << "}" << std::endl;
    }
    if (auto union_type = node->as<ast::UnionDef>())
    {
        ind(out, indent);
        out << "union " << union_type->get_name() << std::endl;
        ind(out, indent + 1);
        out << "switch ( " << union_type->get_union_type() << " )" << std::endl;

        ind(out, indent);
        out << "{" << std::endl;

        for (const auto node : union_type->get_children())
        {
            print_nodes(out, node, indent + 1);
            out << std::endl;
        }

        ind(out, indent);
        out << "}";
    }
    if (auto excp_type = node->as<ast::ExceptionDef>())
    {
        ind(out, indent);
        out << "exception " << excp_type->get_name() << " {" << std::endl;

        for (const auto node : excp_type->get_children())
        {
            print_nodes(out, node, indent + 1);
            out << std::endl;
        }

        ind(out, indent);
        out << "}" << std::endl;
    }
    else if (auto union_case = node->as<ast::CaseDef>())
    {
        for (const auto &label : union_case->get_labels())
        {
            ind(out, indent);
            if (label == "default")
            {
                out << label << ":" << std::endl;
            }
            else
            {
                out << "case " << label << ":" << std::endl;
            }
        }

        ind(out, indent + 1);
        out << union_case->get_case_type() << " " << union_case->get_name() << ";" << std::endl;
    }
    else if (auto var = node->as<ast::VariableDef>())
    {
        ind(out, indent);
        out << "var ";

        if (auto name = var->get_type()->as<ast::Name>())
        {
            out << name->get_name();
        }
        else if (auto node_type = var->get_type()->as<ast::NodeType>())
        {
            out << node_type->get_value();
        }
        else
        {
            assert(false && "Not implemented");
        }

        out << ":" << var->get_name();

        if (auto value = var->get_value())
        {
            out << " = ";
            print_nodes(out, value);
        }
    }
    else if (auto forward = node->as<ast::ForwardDecl>())
    {
        ind(out, indent);
        out << "forw ";

        auto scope_type = forward->get_scope();
        if (scope_type != ast::InterfaceScope::Undef)
        {
            out << to_string(scope_type) << " ";
        }

        out << to_string(forward->get_scope_type()) << " " << forward->get_name() << ";"
            << std::endl;
    }
    else if (auto attr = node->as<ast::AttributeDef>())
    {
        ind(out, indent);
        out << "attr " << attr->get_attr_type() << ":" << attr->get_name();
    }
    else if (auto member = node->as<ast::MemberDef>())
    {
        ind(out, indent);
        out << "memb " << member->get_member_type() << ":" << member->get_name();

        if (auto dim = member->get_array_dimension())
        {
            out << "[";
            print_nodes(out, dim);
            out << "]";
        }
    }
    else if (auto name = node->as<ast::Name>())
    {
        ind(out, indent);
        out << name->get_name();
    }
    else if (auto op = node->as<ast::OperationDef>())
    {
        ind(out, indent);
        out << "op ";

        if (op->is_oneway())
        {
            out << "oneway ";
        }

        out << op->get_return_type() << " " << op->get_name() << "(";

        size_t index = 0;
        for (const auto &param : op->get_params())
        {
            if (index > 0)
            {
                out << ", ";
            }

            out << to_string(param->get_direction()) << " " << param->get_param_type() << ":"
                << param->get_name();
            ++index;
        }

        out << ")";

        if (const auto &raises = op->get_raises(); !raises.empty())
        {
            out << " raises ";
            size_t index = 0;
            for (const auto &name : raises)
            {
                if (index > 0)
                {
                    out << ", ";
                }

                size_t name_index = 0;
                for (const auto &part : name)
                {
                    if (name_index > 0)
                    {
                        out << "::";
                    }
                    out << part;

                    ++name_index;
                }

                ++index;
            }
        }
    }
    else if (auto type_def = node->as<ast::TypeDef>())
    {
        ind(out, indent);
        out << "typedef ";
        print_nodes(out, type_def->get_typedef());
        out << " " << type_def->get_name() << std::endl;
    }
    else if (auto binary = node->as<ast::Binary>())
    {
        print_nodes(out, binary->get_left());
        out << " " << to_string(binary->get_op_type()) << " ";
        print_nodes(out, binary->get_right());
    }
    else if (auto unary = node->as<ast::Unary>())
    {
        if (unary->get_op_type() == ast::OpType::NAV)
        {
            out << "( ";
            print_nodes(out, unary->get_left());
            out << " )";
        }
        else
        {
            out << to_string(unary->get_op_type());
            print_nodes(out, unary->get_left());
        }
    }
    else if (auto seq = node->as<ast::SequenceDef>())
    {
        out << "sequence<";
        print_nodes(out, seq->get_sequence_type());
        if (auto seq_size = seq->get_sequence_size())
        {
            out << ", ";
            print_nodes(out, seq_size);
        }
        out << ">";
    }
    else if (auto literal = node->as<ast::Literal>())
    {
        out << literal->get_value();
    }
    else if (auto node_type = node->as<ast::NodeType>())
    {
        out << node_type->get_value();
    }
}

} // namespace utils
