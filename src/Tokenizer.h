#pragma once

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

enum class TokenType { NONE, IDENTIFIER, NUMBER, STRING, COMA, COLON };

struct Token {
    TokenType token_type = TokenType::NONE;
    std::string_view token_data;

    Token() = default;

    Token(TokenType type, std::string_view data)
        : token_type(type), token_data(data) {}

    Token(const Token &other)
        : token_type(other.token_type), token_data(other.token_data) {}

    Token &operator=(const Token &other) {
        token_type = other.token_type;
        token_data = other.token_data;
        return *this;
    }
};

auto tokenizer(const std::string_view &line, const unsigned int &lineno)
    -> std::vector<Token>;
