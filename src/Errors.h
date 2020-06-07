#pragma once
#include <stdexcept>

#define PARSE_ERR(line, msg)                                                   \
    throw std::runtime_error(msg + std::string(" Line: ") +                    \
                             std::to_string(line))

