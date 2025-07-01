#include <string>
#include <stdio.h>

#include "token.hpp"

std::string display_tokentype(TokenType type) {
    switch (type) {
        case ERROR       : return "ERROR";
        case EOL         : return "EOL";
        case EOC         : return "EOC";
        case LAMBDA      : return "LAMBDA";
        case DOT         : return "DOT";
        case LPAREN      : return "LPAREN";
        case RPAREN      : return "RPAREN";
        case EQUALS      : return "EQUALS";
        case VARIABLE    : return "VARIABLE";
        case IMPORT      : return "IMPORT";
        default          : return "";
    }
}

void print_lexeme(Token self) {
    printf("Lexeme: '%.*s'\n", (int)(self.end-self.start), self.lexeme);
}

void print_token(Token self) {
    printf("[%s] %.*s %li..%li\n", display_tokentype(self.type), (int)(self.end-self.start), self.lexeme, self.start, self.end);
}
