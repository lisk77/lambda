#ifndef TOKEN_H
#define TOKEN_H

#include <stdint.h>
#include <stdlib.h>
#include <string>

typedef enum {
    ERROR,
    EOL,
    EOC,
    LAMBDA,
    DOT,
    LPAREN,
    RPAREN,
    VARIABLE,
    EQUALS
} TokenType;

typedef struct {
    TokenType type;
    std::string lexeme;
    size_t start;
    size_t end;
} Token;

void print_lexeme(Token);
std::string display_tokentype(TokenType);
void print_token(Token);

#endif // TOKEN_H
