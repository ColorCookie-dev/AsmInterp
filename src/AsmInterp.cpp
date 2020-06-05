#include "AsmInterp.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

static auto tokenizer(const std::string &line) -> std::vector<std::string>;

#define PARSE_ERR(line, msg)                                                   \
    throw std::runtime_error(msg + std::string(" Line: ") +                    \
                             std::to_string(line))

auto assembler(const std::vector<std::string> &program)
    -> std::unordered_map<std::string, int> {
    // Registers
    std::unordered_map<std::string, int> regs;

    unsigned int lineno = 0;

    // Helper lambdas
    auto get_reg = [&regs, &lineno](const std::string &reg) -> int & {
        // Checking if the argument is a register or not
        if (!std::all_of(reg.cbegin(), reg.cend(), isalpha))
            PARSE_ERR(lineno, "Unknown argument " + reg + " provided.");

        if (auto search = regs.find(reg); search != regs.end()) {
            return regs[reg];
        } else
            PARSE_ERR(lineno, "Unknown register " + reg + " accessed.");
    };

    auto isnumber = [](const std::string &token) {
        for (auto it = token.begin(); it != token.end(); it++) {
            if ((it == token.begin()) && ((*it == '-') || (*it == '+')))
                continue;
            if (!std::isdigit(*it))
                return false;
        }
        return true;
    };

    auto parse_value = [&regs, &lineno, &isnumber](const std::string &token) {
        if (isnumber(token)) { // Constant
            return std::stoi(token);
        } else if (std::all_of(token.cbegin(), token.cend(),
                               isalpha)) { // Register
            if (auto search = regs.find(token); search != regs.end()) {
                return search->second;
            } else
                PARSE_ERR(lineno, "Unknown register " + token + " accessed.");
        } else {
            PARSE_ERR(lineno, "Unknown paramemter " + token + " given.");
        }
    };

    for (auto it = program.begin(); it != program.end(); it++) {
        std::vector<std::string> tokens(tokenizer(*it));

        if (!tokens.size())
            continue;

        lineno = it - program.begin() + 1;
        if (auto ins = tokens[0]; ins == "mov") {
            if (tokens.size() != 3) // Arguments check
                PARSE_ERR(lineno, "Not enough arguments to 'mov' instruction.");

            // Checking if the last parameter is a register or a number
            regs[tokens[1]] = parse_value(tokens[2]);
        } else if (ins == "inc") {
            if (tokens.size() != 2) // Arguments check
                PARSE_ERR(lineno, "Not enough arguments to 'inc' instruction.");

            get_reg(tokens[1])++;
        } else if (ins == "dec") {
            if (tokens.size() != 2) // Arguments check
                PARSE_ERR(lineno, "Not enough arguments to 'dec' instruction.");

            get_reg(tokens[1])--;
        } else if (ins == "jnz") {
            if (tokens.size() != 3) // Arguments check
                PARSE_ERR(lineno, "Not enough arguments to 'dec' instruction.");

            if (parse_value(tokens[1])) { // if x != 0
                if (auto y = parse_value(tokens[2]);
                    ((it + y) >= program.begin()) && (it + y < program.end())) {
                    it = it + y - 1; // Compensating for the fact that for loop
                                     // with increment the iterator
                    continue;
                } else {
                    PARSE_ERR(lineno, "Jump references a line out of bound!");
                }
            }
        } else { // Unknown instruction
            PARSE_ERR(lineno, "Unknown Instruction Found.");
        }
    }

    return regs;
}

// ----------- Tokenizer

static auto tokenizer(const std::string &line) -> std::vector<std::string> {
    std::vector<std::string> tokens;

    auto istermend = [](unsigned char c) -> bool {
        return ((std::isblank(c) || ispunct(c)) && (c != '-') && (c != '+'));
    };

    auto last_it = line.begin();
    auto it = std::find_if(line.begin(), line.end(), istermend);
    while (it != line.end()) {
        if (*it == ';')
            break;

        if (!istermend(*(it - 1)) && it != line.begin()) {
            auto token = line.substr(last_it - line.begin(), it - last_it);
            tokens.push_back(std::move(token));
        }

        last_it = it + 1;
        it = std::find_if(it + 1, line.end(), istermend);
    }

    if (!istermend(*(it - 1)) &&
        it != line.begin()) { // Inserting last term into tokens
        auto token = line.substr(last_it - line.begin(), it - last_it);
        tokens.push_back(std::move(token));
    }

    return tokens;
}

// --------- End Tokenizer
