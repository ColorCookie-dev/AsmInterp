#pragma once
#include <string>
#include <unordered_map>
#include <vector>

auto assembler(const std::vector<std::string> &program)
    -> std::unordered_map<std::string, int>;
