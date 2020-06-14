#pragma once
#include <stdexcept>

#define PARSE_ERR(line, msg)                                                   \
    throw std::runtime_error(std::string() + msg + " Line: " +                    \
                             std::to_string(line))

