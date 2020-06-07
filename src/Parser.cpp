#include "Errors.h"
#include "Parser.h"

#include <string>
#include <vector>

static auto parse_parameters(std::vector<Token>::iterator start_it,
                             std::vector<Token>::iterator end_it,
                             unsigned int lineno) -> std::vector<Parameter> {
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
