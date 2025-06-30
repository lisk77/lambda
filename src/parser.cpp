#include "parser.hpp"
#include "ast.hpp"

#include <stdexcept>
#include <vector>
#include <string>
#include <memory>
#include <iostream>

Parser::Parser(std::vector<Token> toks)
    : tokens(std::move(toks))
    , position(0)
    , error_flag(false)
    , defs()
{}

const std::unordered_map<std::string, std::unique_ptr<Expr>>& Parser::definitions() const {
    return defs;
}

const Token& Parser::peek() const {
    if (position >= tokens.size())
        throw std::runtime_error("parser: unexpected end of file");
    return tokens[position];
}

const Token& Parser::get() {
    if (position >= tokens.size())
        throw std::runtime_error("parser: unexpected end of file");
    return tokens[position++];
}

bool Parser::accept(TokenType t) {
    if (position < tokens.size() && tokens[position].type == t) {
        ++position;
        return true;
    }
    return false;
}

void Parser::expect(TokenType t) {
    if (!accept(t)) {
        std::cout << display_tokentype(t) << std::endl;
        throw std::runtime_error("parser: unexpected token at position " + std::to_string(peek().start));
    }
}

// parse expressions like "main = \ x . x;"
bool Parser::tryParseDefinition() {
    if (peek().type == TokenType::VARIABLE && position+1 < tokens.size() && tokens[position+1].type == TokenType::EQUALS) {
        std::string name = get().lexeme;
        expect(TokenType::EQUALS);

        std::unique_ptr<Expr> value = parseApplication();

        expect(TokenType::EOL);

        defs.emplace(std::move(name), std::move(value));
        return true;
    }
    return false;
}

// parse simple expressions like "\ x . x" or "x" or stuff in parenthesis
std::unique_ptr<Expr> Parser::parseSimple() {
    if (accept(TokenType::LAMBDA)) {
        std::vector<std::string> params;
        while (peek().type == TokenType::VARIABLE) {
            params.push_back(get().lexeme);
        }
        expect(TokenType::DOT);

        std::unique_ptr<Expr> body = parseApplication();
        for (auto it = params.rbegin(); it != params.rend(); ++it) {
            body = std::make_unique<Abstraction>(*it, std::move(body));
        }
        return body;
    }

    if (peek().type == TokenType::VARIABLE) {
        const auto &tok = get();
        return std::make_unique<Variable>(tok.lexeme);
    }

    if (accept(TokenType::LPAREN)) {
        std::unique_ptr<Expr> e = parseApplication();
        expect(TokenType::RPAREN);
        return e;
    }

    throw std::runtime_error("parser: expected \\, variable, or '(' at position " + std::to_string(peek().start));
}

// parse function applications
std::unique_ptr<Expr> Parser::parseApplication() {
    std::unique_ptr<Expr> expr = parseSimple();
    while (true) {
        TokenType t = peek().type;
        if (t == TokenType::VARIABLE || t == TokenType::LAMBDA || t == TokenType::LPAREN) {
            std::unique_ptr<Expr> rhs = parseSimple();
            expr = std::make_unique<Application>(std::move(expr), std::move(rhs));
        }
        else break;
    }
    return expr;
}

// uuum... i think the name says it?
std::vector<std::unique_ptr<Expr>> Parser::parseProgram() {
    std::vector<std::unique_ptr<Expr>> results;

    while (peek().type != TokenType::EOC) {
        if (tryParseDefinition()) continue;

        results.push_back(parseApplication());
        expect(TokenType::EOL);
    }

    expect(TokenType::EOC);
    return results;
}
