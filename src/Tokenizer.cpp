#include "Errors.h"
#include "Tokenizer.h"

constexpr auto is_potential_identifier_start(const unsigned char c) -> bool {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
}

constexpr auto is_potential_identifier_char(const unsigned char c) -> bool {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || c == '_');
}

auto tokenizer(const std::string_view &line, const unsigned int &lineno)
    -> std::vector<Token> {
    std::vector<Token> tokens;

    for (std::string_view::const_iterator it = line.begin(); it != line.end();
         it++) {
        char c = *it;

        if (is_potential_identifier_start(c)) { // Parse identifier
            std::string_view::const_iterator search = std::find_if_not(
                std::next(it), line.end(), is_potential_identifier_char);

            tokens.emplace_back(TokenType::IDENTIFIER,
                                line.substr(it - line.begin(), search - it));
            it = std::prev(search);
        } else if (static_cast<bool>(isdigit(c)) || (c == '+') ||
                   (c == '-')) { // Parse Numbers
            std::string_view::const_iterator search =
                std::find_if_not(std::next(it), line.end(), isdigit);
            tokens.emplace_back(TokenType::NUMBER,
                                line.substr(it - line.begin(), search - it));
            it = std::prev(search);
        } else if (c == '\'') { // Parse string
            std::string_view::const_iterator search =
                std::find_if(std::next(it), line.end(),
                             [](unsigned char c) { return c == '\''; });
            if (search != line.end()) {
                tokens.emplace_back(
                    TokenType::STRING,
                    line.substr(it + 1 - line.begin(), search - it - 1));
                it = search; // search can't be the end here.
            } else {
                PARSE_ERR(lineno, "Unmatched \"'\"");
            }
        } else if (static_cast<bool>(isblank(c))) { // Parse whitespace
            continue;
        } else if (c == ',') { // Parse coma
            tokens.emplace_back(TokenType::COMA, ",");
        } else if (c == ':') { // Parse colon
            tokens.emplace_back(TokenType::COLON, ":");
        } else if (c == ';') { // Parse semicolon
            break;
        } else {
            PARSE_ERR(lineno, "Unknown token passed: '" + c + "'");
        }
    }

    return tokens;
}
