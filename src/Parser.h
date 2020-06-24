#pragma once

#include "Tokenizer.h"

#include <string>
#include <vector>

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
    InstructionType ins_type;
    std::vector<Parameter> paramemters;

    Instruction(InstructionType type) noexcept : ins_type(type) {}

    Instruction(InstructionType type, const std::vector<Parameter> &paramemters) noexcept
        : ins_type(type), paramemters(paramemters) {}

    Instruction(InstructionType type, std::vector<Parameter> &&paramemters) noexcept
        : ins_type(type), paramemters(std::move(paramemters)) {}

    Instruction(const Instruction &other) noexcept
        : ins_type(other.ins_type), paramemters(other.paramemters) {}

    Instruction(Instruction &&other) noexcept
        : ins_type(other.ins_type), paramemters(std::move(other.paramemters)) {}
};

auto parser(std::vector<Token> &tokens, const unsigned int &lineno)
    -> Instruction;
