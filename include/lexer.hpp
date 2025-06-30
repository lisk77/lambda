#ifndef LEXER_HPP
#define LEXER_HPP

#include <vector>
#include <string>
#include "token.hpp"

class Lexer {
    std::string src;
    bool error_flag;
    size_t position;

public:
    Lexer(std::string);
    std::vector<Token> tokenize();
};

#endif // LEXER_HPP
