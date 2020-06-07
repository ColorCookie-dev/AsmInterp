#include "Errors.h"
#include "Tokenizer.h"

#define is_potential_identifier_start(c)                                       \
    ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')

#define is_potential_identifier_char(c)                                        \
    ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||                       \
     (c >= '0' && c <= '9') || c == '_')

auto tokenizer(const std::string &line, unsigned int lineno)
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
