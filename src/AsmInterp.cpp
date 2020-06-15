#include "AsmInterp.h"
#include "Errors.h"
#include "Parser.h"
#include "Tokenizer.h"

#include <algorithm>
#include <charconv>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

auto assembler_interpreter(const std::string_view &program_source)
    -> std::string {

    std::vector<std::string_view> program;

    // Splitting strings into lines
    for (auto search =
                  std::find(begin(program_source), end(program_source), '\n'),
              last_iter = begin(program_source);
         last_iter != end(program_source);
         last_iter =
             (search == end(program_source) ? search : std::next(search)),
              search = std::find(last_iter, end(program_source), '\n')) {
        program.push_back(program_source.substr(
            last_iter - begin(program_source), search - last_iter));
    }

    // Registers
    std::unordered_map<std::string_view, int> regs;
    std::unordered_map<std::string_view, size_t> label_defs;

    unsigned int lineno = 0;

    // Helper lambdas
    const auto get_reg = [&regs, &lineno](const std::string_view &reg) -> int & {
        if (auto search = regs.find(reg); search != regs.end()) {
            return regs[reg];
        }

        PARSE_ERR(lineno,
                  "Unknown register " + std::string(reg) + " accessed.");
    };

    const auto get_label = [&label_defs,
                            &lineno](const std::string_view &label) -> size_t & {
        if (auto search = label_defs.find(label); search != label_defs.end()) {
            return label_defs[label];
        }

        PARSE_ERR(lineno,
                  "Unknown register " + std::string(label) + " accessed.");
    };

    const auto parse_val = [&lineno,
                            &get_reg](const Parameter &paramemter) -> int {
        int parsed_val = 0;
        const auto tok_data = paramemter.token_data;

        if (paramemter.token_type == TokenType::NUMBER) {
            if (const auto [p, ec] = std::from_chars(
                    tok_data.data(), tok_data.data() + tok_data.size(),
                    parsed_val);
                ec == std::errc()) {
                return parsed_val;
            }
            PARSE_ERR(lineno, "Unable to convert string to integer!");
        }

        return get_reg(paramemter.token_data);
    };

    std::vector<Instruction> program_ast;
    for (auto it = program.begin(); it != program.end(); it++) {
        std::vector<Token> tokens(tokenizer(*it, lineno));

        if (tokens.empty()) {
            continue;
        }

        lineno = it - program.begin() + 1;

        auto instruction =
            parser(tokens, lineno); // For moving purposes it's not const

        // Label definitions
        if (instruction.ins_type == InstructionType::LABEL) {
            if (label_defs.find(instruction.paramemters[0].token_data) ==
                label_defs.end()) {
                label_defs[instruction.paramemters[0].token_data] =
                    program_ast.size();
            } else {
                PARSE_ERR(lineno, "Label redeclaration error");
            }
        }

        program_ast.push_back(std::move(instruction));
    }

    std::stack<size_t> stack; // Currently only for pushing return locations
    int cmp_test = 0;
    bool program_ended = false;
    std::string output;

    // The instructions runs here
    for (auto it = program_ast.begin();
         (it != program_ast.end() && !program_ended); it++) {

        switch (it->ins_type) {

        case InstructionType::MOV:
            regs[it->paramemters[0].token_data] = parse_val(it->paramemters[1]);
            break;

        case InstructionType::INC:
            get_reg(it->paramemters[0].token_data)++;
            break;

        case InstructionType::DEC:
            get_reg(it->paramemters[0].token_data)--;
            break;

        case InstructionType::ADD:
            get_reg(it->paramemters[0].token_data) +=
                parse_val(it->paramemters[1]);
            break;

        case InstructionType::SUB:
            get_reg(it->paramemters[0].token_data) -=
                parse_val(it->paramemters[1]);
            break;

        case InstructionType::MUL:
            get_reg(it->paramemters[0].token_data) *=
                parse_val(it->paramemters[1]);
            break;

        case InstructionType::DIV:
            if (auto parsed_val = parse_val(it->paramemters[1]);
                parsed_val == 0) {
                PARSE_ERR(lineno, "Division by Zero");
            } else {
                get_reg(it->paramemters[0].token_data) /= parsed_val;
            }
            break;

        case InstructionType::CMP:
            cmp_test =
                parse_val(it->paramemters[0]) - parse_val(it->paramemters[1]);
            break;

        case InstructionType::JMP:
            it = program_ast.begin() + get_label(it->paramemters[0].token_data);
            break;

        case InstructionType::JNE:
            if (cmp_test != 0) {
                it = program_ast.begin() +
                     get_label(it->paramemters[0].token_data);
            }
            break;

        case InstructionType::JE:
            if (cmp_test == 0) {
                it = program_ast.begin() +
                     get_label(it->paramemters[0].token_data);
            }
            break;

        case InstructionType::JGE:
            if (cmp_test >= 0) {
                it = program_ast.begin() +
                     get_label(it->paramemters[0].token_data);
            }
            break;

        case InstructionType::JG:
            if (cmp_test > 0) {
                it = program_ast.begin() +
                     get_label(it->paramemters[0].token_data);
            }
            break;

        case InstructionType::JLE:
            if (cmp_test <= 0) {
                it = program_ast.begin() +
                     get_label(it->paramemters[0].token_data);
            }
            break;

        case InstructionType::JL:
            if (cmp_test < 0) {
                it = program_ast.begin() +
                     get_label(it->paramemters[0].token_data);
            }
            break;

        case InstructionType::CALL:
            stack.push(it - program_ast.begin());
            it = program_ast.begin() + get_label(it->paramemters[0].token_data);
            break;

        case InstructionType::RET:
            if (stack.empty()) {
                PARSE_ERR(lineno, "Nowhere to return!");
            }
            it = program_ast.begin() + stack.top();
            stack.pop();
            break;

        case InstructionType::MSG:
            for (auto &paramemter : it->paramemters) {
                if (paramemter.token_type == TokenType::STRING) {
                    output += paramemter.token_data;
                } else {
                    output += std::to_string(parse_val(paramemter));
                }
            }
            break;

        case InstructionType::END:
            program_ended = true;
            break;

        case InstructionType::LABEL:
            break;
        }
    }

    return program_ended ? output : "-1";
}
