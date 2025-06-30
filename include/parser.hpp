#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <unordered_map>

#include "token.hpp"
#include "ast.hpp"

class Parser {
    std::vector<Token> tokens;
    size_t position;
    bool error_flag;
    std::unordered_map<std::string, std::unique_ptr<Expr>> defs;

public:
    Parser(std::vector<Token>);
    const std::unordered_map<std::string, std::unique_ptr<Expr>>& definitions() const;
    const Token& peek() const;
    const Token& get();
    bool accept(TokenType t);
    void expect(TokenType t);
    bool tryParseDefinition();
    std::unique_ptr<Expr> parseSimple();
    std::unique_ptr<Expr> parseApplication();
    std::unique_ptr<Expr> parseTerm();
    std::unique_ptr<Expr> parse();
    std::vector<std::unique_ptr<Expr>> parseProgram();
};

#endif // PARSER_HPP
