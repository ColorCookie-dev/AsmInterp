#include "AsmInterp.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#define PARSE_ERR(line, msg)                                                   \
    throw std::runtime_error(msg + std::string(" Line: ") +                    \
                             std::to_string(line))

// ----------- Tokenizer
//
enum class TokenType { NONE, IDENTIFIER, NUMBER, STRING, COMA, COLON };

struct Token {
    TokenType token_type = TokenType::NONE;
    std::string token_data;

    Token() {}

    Token(TokenType type, const std::string &data)
        : token_type(type), token_data(data) {}

    Token(TokenType type, std::string &&data)
        : token_type(type), token_data(std::move(data)) {}

    Token(const Token &other)
        : token_type(other.token_type), token_data(other.token_data) {}

    Token(Token &&other)
        : token_type(other.token_type),
          token_data(std::move(other.token_data)) {}
};

#define is_potential_identifier_start(c)                                       \
    ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')

#define is_potential_identifier_char(c)                                        \
    ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||                       \
     (c >= '0' && c <= '9') || c == '_')

static auto tokenizer(const std::string &line, unsigned int lineno)
    -> std::vector<Token> {
    std::vector<Token> tokens;

    for (auto it = line.begin(); it != line.end(); it++) {
        char c = *it;

        if (is_potential_identifier_start(c)) { // Parse identifier
            auto search =
                std::find_if_not(it + 1, line.end(), [](unsigned char c) {
                    return is_potential_identifier_char(c);
                });
            tokens.emplace_back(TokenType::IDENTIFIER,
                                line.substr(it - line.begin(), search - it));
            it = search - 1;
        } else if (isdigit(c) || (c == '+') || (c == '-')) { // Parse Numbers
            auto search = std::find_if_not(it + 1, line.end(), isdigit);
            tokens.emplace_back(TokenType::NUMBER,
                                line.substr(it - line.begin(), search - it));
            it = search - 1;
        } else if (c == '\'') { // Parse string
            auto search = std::find_if(
                it + 1, line.end(), [](unsigned char c) { return c == '\''; });
            if (search != line.end()) {
                tokens.emplace_back(
                    TokenType::STRING,
                    line.substr(it + 1 - line.begin(), search - it - 1));
                it = search; // search can't be the end here.
            } else {
                PARSE_ERR(lineno, "Unmatched \"'\"");
            }
        } else if (isblank(c)) { // Parse whitespace
            continue;
        } else if (c == ',') { // Parse coma
            tokens.emplace_back(TokenType::COMA, ",");
        } else if (c == ':') { // Parse colon
            tokens.emplace_back(TokenType::COLON, ":");
        } else if (c == ';') { // Parse semicolon
            break;
        } else {
            PARSE_ERR(lineno,
                      std::string() + "Unknown token passed: '" + c + "'");
        }
    }

    return tokens;
}

// --------- End Tokenizer

// ----------- Lexer

enum class InstructionType {
    MOV,
    INC,
    DEC,
    ADD,
    SUB,
    MUL,
    DIV,
    LABEL,
    JMP,
    CMP,
    JNE,
    JE,
    JGE,
    JG,
    JLE,
    JL,
    CALL,
    RET,
    MSG,
    END
};

using Parameter = Token;

struct Instruction {
    const InstructionType ins_type;
    std::vector<Parameter> paramemters;

    Instruction(InstructionType type) : ins_type(type) {}

    Instruction(InstructionType type, const std::vector<Parameter> &paramemters)
        : ins_type(type), paramemters(paramemters) {}

    Instruction(InstructionType type, std::vector<Parameter> &&paramemters)
        : ins_type(type), paramemters(std::move(paramemters)) {}

    Instruction(const Instruction &other)
        : ins_type(other.ins_type), paramemters(other.paramemters) {}

    Instruction(Instruction &&other)
        : ins_type(other.ins_type), paramemters(std::move(other.paramemters)) {}
};

auto parse_parameters(std::vector<Token>::iterator start_it,
                      std::vector<Token>::iterator end_it, unsigned int lineno)
    -> std::vector<Parameter> {
    std::vector<Parameter> paramemters;

    for (auto it = start_it; it != end_it; it++) {
        if (TokenType tok_type = it->token_type;
            (tok_type == TokenType::IDENTIFIER) ||
            (tok_type == TokenType::NUMBER) ||
            (tok_type == TokenType::STRING)) {
            if (it + 1 != end_it) {
                if (std::next(it)->token_type == TokenType::COMA) {
                    paramemters.push_back(std::move(*it));
                    it++;
                } else {
                    PARSE_ERR(lineno, "Error parsing '" +
                                          std::next(it)->token_data +
                                          "', coma ',' expected!");
                }
            } else { // End of paramemters
                paramemters.push_back(std::move(*it));
            }
        } else {
            PARSE_ERR(lineno, "Error parsing '" + it->token_data + "'");
        }
    }

    return paramemters;
}

auto parser(std::vector<Token> &tokens, unsigned int lineno) -> Instruction {
    if (!tokens.size())
        PARSE_ERR(lineno, "Nothing to parse! This error shouldn't happen, "
                          "Implementation error!");

    auto first_tok = tokens[0];

    if (first_tok.token_type != TokenType::IDENTIFIER) {
        PARSE_ERR(lineno, "No instruction given");
    }

    // Parsing the instruction type first
    if (auto ins = first_tok.token_data; ins == "mov") {
        auto paramemters =
            parse_parameters(tokens.begin() + 1, tokens.end(), lineno);
        if (paramemters.size() != 2) // Arguments check
            PARSE_ERR(lineno, "'mov' instruction requires 2 arguments, given " +
                                  std::to_string(paramemters.size()) + ".");

        if (!((paramemters[0].token_type == TokenType::IDENTIFIER) &&
              ((paramemters[1].token_type == TokenType::IDENTIFIER) ||
               (paramemters[1].token_type == TokenType::NUMBER)))) {
            PARSE_ERR(lineno, "Invalid arguments given to 'mov' instruction.");
        }

        return {InstructionType::MOV, std::move(paramemters)};

    } else if (ins == "inc") {
        auto paramemters =
            parse_parameters(tokens.begin() + 1, tokens.end(), lineno);
        if (paramemters.size() != 1) // Arguments check
            PARSE_ERR(lineno, "'inc' instruction requires 1 arguments, given " +
                                  std::to_string(paramemters.size()) + ".");

        if (paramemters[0].token_type != TokenType::IDENTIFIER) {
            PARSE_ERR(lineno, "Invalid arguments given to 'inc' instruction.");
        }

        return {InstructionType::INC, std::move(paramemters)};

    } else if (ins == "dec") {
        auto paramemters =
            parse_parameters(tokens.begin() + 1, tokens.end(), lineno);
        if (paramemters.size() != 1) // Arguments check
            PARSE_ERR(lineno, "'dec' instruction requires 1 arguments, given " +
                                  std::to_string(paramemters.size()) + ".");

        if (paramemters[0].token_type != TokenType::IDENTIFIER) {
            PARSE_ERR(lineno, "Invalid arguments given to 'dec' instruction.");
        }

        return {InstructionType::DEC, std::move(paramemters)};

    } else if (auto ins = first_tok.token_data; ins == "add") {
        auto paramemters =
            parse_parameters(tokens.begin() + 1, tokens.end(), lineno);
        if (paramemters.size() != 2) // Arguments check
            PARSE_ERR(lineno, "'add' instruction requires 2 arguments, given " +
                                  std::to_string(paramemters.size()) + ".");

        if (!((paramemters[0].token_type == TokenType::IDENTIFIER) &&
              ((paramemters[1].token_type == TokenType::IDENTIFIER) ||
               (paramemters[1].token_type == TokenType::NUMBER)))) {
            PARSE_ERR(lineno, "Invalid arguments given to 'add' instruction.");
        }

        return {InstructionType::ADD, std::move(paramemters)};

    } else if (auto ins = first_tok.token_data; ins == "sub") {
        auto paramemters =
            parse_parameters(tokens.begin() + 1, tokens.end(), lineno);
        if (paramemters.size() != 2) // Arguments check
            PARSE_ERR(lineno, "'sub' instruction requires 2 arguments, given " +
                                  std::to_string(paramemters.size()) + ".");

        if (!((paramemters[0].token_type == TokenType::IDENTIFIER) &&
              ((paramemters[1].token_type == TokenType::IDENTIFIER) ||
               (paramemters[1].token_type == TokenType::NUMBER)))) {
            PARSE_ERR(lineno, "Invalid arguments given to 'sub' instruction.");
        }

        return {InstructionType::SUB, std::move(paramemters)};

    } else if (auto ins = first_tok.token_data; ins == "mul") {
        auto paramemters =
            parse_parameters(tokens.begin() + 1, tokens.end(), lineno);
        if (paramemters.size() != 2) // Arguments check
            PARSE_ERR(lineno, "'mul' instruction requires 2 arguments, given " +
                                  std::to_string(paramemters.size()) + ".");

        if (!((paramemters[0].token_type == TokenType::IDENTIFIER) &&
              ((paramemters[1].token_type == TokenType::IDENTIFIER) ||
               (paramemters[1].token_type == TokenType::NUMBER)))) {
            PARSE_ERR(lineno, "Invalid arguments given to 'mul' instruction.");
        }

        return {InstructionType::MUL, std::move(paramemters)};

    } else if (auto ins = first_tok.token_data; ins == "div") {
        auto paramemters =
            parse_parameters(tokens.begin() + 1, tokens.end(), lineno);
        if (paramemters.size() != 2) // Arguments check
            PARSE_ERR(lineno, "'div' instruction requires 2 arguments, given " +
                                  std::to_string(paramemters.size()) + ".");

        if (!((paramemters[0].token_type == TokenType::IDENTIFIER) &&
              ((paramemters[1].token_type == TokenType::IDENTIFIER) ||
               (paramemters[1].token_type == TokenType::NUMBER)))) {
            PARSE_ERR(lineno, "Invalid arguments given to 'div' instruction.");
        }

        return {InstructionType::DIV, std::move(paramemters)};

    } else if (auto ins = first_tok.token_data; ins == "cmp") {
        auto paramemters =
            parse_parameters(tokens.begin() + 1, tokens.end(), lineno);
        if (paramemters.size() != 2) // Arguments check
            PARSE_ERR(lineno, "'cmp' instruction requires 2 arguments, given " +
                                  std::to_string(paramemters.size()) + ".");

        if (!(((paramemters[0].token_type == TokenType::IDENTIFIER) ||
               (paramemters[0].token_type == TokenType::NUMBER)) &&
              ((paramemters[1].token_type == TokenType::IDENTIFIER) ||
               (paramemters[1].token_type == TokenType::NUMBER)))) {
            PARSE_ERR(lineno, "Invalid arguments given to 'cmp' instruction.");
        }

        return {InstructionType::CMP, std::move(paramemters)};

    } else if (ins == "jmp") {
        auto paramemters =
            parse_parameters(tokens.begin() + 1, tokens.end(), lineno);
        if (paramemters.size() != 1) // Arguments check
            PARSE_ERR(lineno, "'jmp' instruction requires 1 arguments, given " +
                                  std::to_string(paramemters.size()) + ".");

        if (paramemters[0].token_type != TokenType::IDENTIFIER) {
            PARSE_ERR(lineno, "Invalid arguments given to 'jmp' instruction.");
        }

        return {InstructionType::JMP, std::move(paramemters)};

    } else if (ins == "jne") {
        auto paramemters =
            parse_parameters(tokens.begin() + 1, tokens.end(), lineno);
        if (paramemters.size() != 1) // Arguments check
            PARSE_ERR(lineno, "'jne' instruction requires 1 arguments, given " +
                                  std::to_string(paramemters.size()) + ".");

        if (paramemters[0].token_type != TokenType::IDENTIFIER) {
            PARSE_ERR(lineno, "Invalid arguments given to 'jne' instruction.");
        }

        return {InstructionType::JNE, std::move(paramemters)};

    } else if (ins == "je") {
        auto paramemters =
            parse_parameters(tokens.begin() + 1, tokens.end(), lineno);
        if (paramemters.size() != 1) // Arguments check
            PARSE_ERR(lineno, "'je' instruction requires 1 arguments, given " +
                                  std::to_string(paramemters.size()) + ".");

        if (paramemters[0].token_type != TokenType::IDENTIFIER) {
            PARSE_ERR(lineno, "Invalid arguments given to 'je' instruction.");
        }

        return {InstructionType::JE, std::move(paramemters)};

    } else if (ins == "jge") {
        auto paramemters =
            parse_parameters(tokens.begin() + 1, tokens.end(), lineno);
        if (paramemters.size() != 1) // Arguments check
            PARSE_ERR(lineno, "'jge' instruction requires 1 arguments, given " +
                                  std::to_string(paramemters.size()) + ".");

        if (paramemters[0].token_type != TokenType::IDENTIFIER) {
            PARSE_ERR(lineno, "Invalid arguments given to 'jge' instruction.");
        }

        return {InstructionType::JGE, std::move(paramemters)};

    } else if (ins == "jg") {
        auto paramemters =
            parse_parameters(tokens.begin() + 1, tokens.end(), lineno);
        if (paramemters.size() != 1) // Arguments check
            PARSE_ERR(lineno, "'jg' instruction requires 1 arguments, given " +
                                  std::to_string(paramemters.size()) + ".");

        if (paramemters[0].token_type != TokenType::IDENTIFIER) {
            PARSE_ERR(lineno, "Invalid arguments given to 'jg' instruction.");
        }

        return {InstructionType::JG, std::move(paramemters)};

    } else if (ins == "jle") {
        auto paramemters =
            parse_parameters(tokens.begin() + 1, tokens.end(), lineno);
        if (paramemters.size() != 1) // Arguments check
            PARSE_ERR(lineno, "'jle' instruction requires 1 arguments, given " +
                                  std::to_string(paramemters.size()) + ".");

        if (paramemters[0].token_type != TokenType::IDENTIFIER) {
            PARSE_ERR(lineno, "Invalid arguments given to 'jle' instruction.");
        }

        return {InstructionType::JLE, std::move(paramemters)};

    } else if (ins == "jl") {
        auto paramemters =
            parse_parameters(tokens.begin() + 1, tokens.end(), lineno);
        if (paramemters.size() != 1) // Arguments check
            PARSE_ERR(lineno, "'jl' instruction requires 1 arguments, given " +
                                  std::to_string(paramemters.size()) + ".");

        if (paramemters[0].token_type != TokenType::IDENTIFIER) {
            PARSE_ERR(lineno, "Invalid arguments given to 'jl' instruction.");
        }

        return {InstructionType::JL, std::move(paramemters)};

    } else if (ins == "call") {
        auto paramemters =
            parse_parameters(tokens.begin() + 1, tokens.end(), lineno);
        if (paramemters.size() != 1) // Arguments check
            PARSE_ERR(lineno,
                      "'call' instruction requires 1 arguments, given " +
                          std::to_string(paramemters.size()) + ".");

        if (paramemters[0].token_type != TokenType::IDENTIFIER) {
            PARSE_ERR(lineno, "Invalid arguments given to 'call' instruction.");
        }

        return {InstructionType::CALL, std::move(paramemters)};

    } else if (ins == "msg") {
        auto paramemters =
            parse_parameters(tokens.begin() + 1, tokens.end(), lineno);
        // No argument length check
        for (auto &paramemter : paramemters) {
            if ((paramemter.token_type != TokenType::IDENTIFIER) &&
                (paramemter.token_type != TokenType::STRING) &&
                (paramemter.token_type != TokenType::NUMBER)) {
                PARSE_ERR(lineno,
                          "Invalid arguments given to 'msg' instruction.");
            }
        }

        return {InstructionType::MSG, std::move(paramemters)};

    } else if (ins == "ret") {
        auto paramemters =
            parse_parameters(tokens.begin() + 1, tokens.end(), lineno);
        if (paramemters.size() != 0) // Arguments check
            PARSE_ERR(lineno, "'ret' instruction requires 0 arguments, given " +
                                  std::to_string(paramemters.size()) + ".");

        return {InstructionType::RET, std::move(paramemters)};

    } else if (ins == "end") {
        auto paramemters =
            parse_parameters(tokens.begin() + 1, tokens.end(), lineno);
        if (paramemters.size() != 0) // Arguments check
            PARSE_ERR(lineno, "'end' instruction requires 0 arguments, given " +
                                  std::to_string(paramemters.size()) + ".");

        return {InstructionType::END, std::move(paramemters)};

    } else if ((tokens.size() == 2) &&
               (tokens[1].token_type == TokenType::COLON)) { // Handling labels
        return {InstructionType::LABEL, {first_tok}};

    } else { // Unknown instruction
        PARSE_ERR(lineno, "Unknown Instruction Found.");
    }
}

// --------- End Parser

auto assembler_interpreter(const std::string &program_source) -> std::string {

    std::vector<std::string> program;
    std::stringstream program_source_stream(program_source);
    std::string line;
    while (std::getline(program_source_stream, line, '\n'))
        program.push_back(std::move(line));

    // Registers
    std::unordered_map<std::string, int> regs;
    std::unordered_map<std::string, size_t> label_defs;

    unsigned int lineno = 0;

    // Helper lambdas
    auto get_reg = [&regs, &lineno](const std::string &reg) -> int & {
        if (auto search = regs.find(reg); search != regs.end()) {
            return regs[reg];
        } else
            PARSE_ERR(lineno, "Unknown register " + reg + " accessed.");
    };

    auto get_label = [&label_defs,
                      &lineno](const std::string &label) -> size_t & {
        if (auto search = label_defs.find(label); search != label_defs.end()) {
            return label_defs[label];
        } else
            PARSE_ERR(lineno, "Unknown register " + label + " accessed.");
    };

    auto parse_val = [&get_reg](Parameter paramemter) -> int {
        return (paramemter.token_type == TokenType::NUMBER)
                   ? std::stoi(paramemter.token_data)
                   : get_reg(paramemter.token_data);
    };

    std::vector<Instruction> program_ast;
    for (auto it = program.begin(); it != program.end(); it++) {
        std::vector<Token> tokens(tokenizer(*it, lineno));

        if (!tokens.size())
            continue;

        lineno = it - program.begin() + 1;
        auto instruction = parser(tokens, lineno);

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
            get_reg(it->paramemters[0].token_data) /=
                parse_val(it->paramemters[1]);
            break;

        case InstructionType::CMP:
            cmp_test =
                parse_val(it->paramemters[0]) - parse_val(it->paramemters[1]);
            break;

        case InstructionType::JMP:
            it = program_ast.begin() + get_label(it->paramemters[0].token_data);
            break;

        case InstructionType::JNE:
            if (cmp_test != 0)
                it = program_ast.begin() +
                     get_label(it->paramemters[0].token_data);
            break;

        case InstructionType::JE:
            if (cmp_test == 0)
                it = program_ast.begin() +
                     get_label(it->paramemters[0].token_data);
            break;

        case InstructionType::JGE:
            if (cmp_test >= 0)
                it = program_ast.begin() +
                     get_label(it->paramemters[0].token_data);
            break;

        case InstructionType::JG:
            if (cmp_test > 0)
                it = program_ast.begin() +
                     get_label(it->paramemters[0].token_data);
            break;

        case InstructionType::JLE:
            if (cmp_test <= 0)
                it = program_ast.begin() +
                     get_label(it->paramemters[0].token_data);
            break;

        case InstructionType::JL:
            if (cmp_test < 0)
                it = program_ast.begin() +
                     get_label(it->paramemters[0].token_data);
            break;

        case InstructionType::CALL:
            stack.push(it - program_ast.begin());
            it = program_ast.begin() + get_label(it->paramemters[0].token_data);
            break;

        case InstructionType::RET:
            if (stack.empty())
                PARSE_ERR(lineno, "Nowhere to return!");
            it = program_ast.begin() + stack.top();
            stack.pop();
            break;

        case InstructionType::MSG:
            for (auto &paramemter : it->paramemters) {
                if (paramemter.token_type == TokenType::STRING)
                    output += paramemter.token_data;
                else
                    output += std::to_string(parse_val(paramemter));
            }
            break;

        case InstructionType::END:
            program_ended = true;
            break;

        case InstructionType::LABEL:
            break;
        }
    }

    if (program_ended)
        return output;
    else
        return "-1";
}
