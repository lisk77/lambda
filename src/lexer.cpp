#include <iostream>

#include "lexer.hpp"

#define IS_SPACE(c) (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v')

Lexer::Lexer(std::string src) {
    this->src = src;
    this->position = 0;
    this->error_flag = false;
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    std::string src = this->src;
    size_t len = src.length();

    while (position < len && !error_flag) {
        char curr = src[position];

        if (curr == '\\') {
            tokens.push_back({ LAMBDA, "\\", position, position+1 });
            position++;
        }
        else if (curr == '.') {
            tokens.push_back({ DOT, ".", position, position+1 });
            position++;
        }
        else if (curr == '(') {
            tokens.push_back({ LPAREN, "(", position, position+1 });
            position++;
        }
        else if (curr == ')') {
            tokens.push_back({ RPAREN, ")", position, position+1 });
            position++;
        }
        else if (curr == '=') {
            tokens.push_back({ EQUALS, "=", position, position+1 });
            position++;
        }
        else if (curr == ';') {
            tokens.push_back({ EOL, ";", position, position+1 });
            position++;
        }
        else if (!IS_SPACE(curr)) {
            size_t start = position;
            while (position < len && !IS_SPACE(src[position])
                   && src[position] != '\\'
                   && src[position] != '('
                   && src[position] != ')' 
                   && src[position] != '.' 
                   && src[position] != '='
                   && src[position] != ';')
            {
                position++;
            }
            std::string lex = src.substr(start, position - start);
            tokens.push_back({ VARIABLE, lex, start, position });
        }
        else if (IS_SPACE(curr)) {
            position++;
        }
        else {
            error_flag = true;
            std::cerr << "Unknown symbol " << curr << std::endl;
            tokens.push_back({ ERROR, std::string (1, curr), position, position });
        }
    }

    tokens.push_back({ EOC, "", position, position });
    return tokens;
}
