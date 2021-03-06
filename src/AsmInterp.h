#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

auto assembler_interpreter(const std::string_view &program_source) -> std::string;
