#pragma once

#include "Parser.hpp"

#include <stdio.h>
#include <memory>

namespace idl
{
class Scanner;
}

struct ParserDriver
{
    ParserDriver();
    ~ParserDriver();

    bool parse(std::istream &in);

    void parse_error(int line, int column, const std::string &msg);

    idl::Scanner &get_scanner();

    auto &get_top_scope()
    {
        return m_top_scope;
    }

    std::unique_ptr<idl::Scanner> m_scanner;
    ast::ScopeDef m_top_scope;
};
