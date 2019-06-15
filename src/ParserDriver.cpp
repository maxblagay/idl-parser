#include "ParserDriver.hpp"

#include <iostream>

void
ParserDriver::parse_error(int line, int column, const std::string &msg)
{
    std::cerr << "Failed to parse with error: " << msg << std::endl;
}
