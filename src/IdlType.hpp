#pragma once

#include "types.hpp"

namespace ast
{
enum class IdlType : uint8_t
{
    NAV,

    Void,
    Short,
    Long,
    LongLong,

    UnsignedShort,
    UnsignedLong,
    UnsignedLongLong,

    Float,
    Double,
    LongDouble,

    Char,
    WChar,

    String,
    WString,

    Boolean,
    Any,
    Octet,
    Object,

    Fixed
};

std::string to_string(IdlType type);

} // namespace ast
