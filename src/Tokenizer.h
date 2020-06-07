#pragma once

#include <algorithm>
#include <string>
#include <vector>

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

auto tokenizer(const std::string &line, unsigned int lineno)
    -> std::vector<Token>;
