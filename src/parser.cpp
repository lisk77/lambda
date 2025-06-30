// src/parser.cpp
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
    throw std::runtime_error("parser: unexpected token at position "
                             + std::to_string(peek().start));
  }
}

// Try to parse a definition of the form:
//   <variable> '=' <term> ';'
// Returns true if a definition was parsed and stored in defs.
bool Parser::tryParseDefinition() {
  if (peek().type == TokenType::VARIABLE
   && position+1 < tokens.size()
   && tokens[position+1].type == TokenType::EQUALS)
  {
    std::string name = get().lexeme;  // consume VARIABLE
    expect(TokenType::EQUALS);            // consume '='

    // parse the right-hand term
    std::unique_ptr<Expr> value = parseTerm();

    // require a semicolon (EOL token)
    expect(TokenType::EOL);

    // store in definitions map
    defs.emplace(std::move(name), std::move(value));
    return true;
  }
  return false;
}

// <simple> ::= '\' <variable>+ '.' <term>
//            | <variable>
//            | '(' <term> ')'
std::unique_ptr<Expr> Parser::parseSimple() {
  // abstraction with one-or-more parameters
  if (accept(TokenType::LAMBDA)) {
    std::vector<std::string> params;
    while (peek().type == TokenType::VARIABLE) {
      params.push_back(get().lexeme);
    }
    expect(TokenType::DOT);

    std::unique_ptr<Expr> body = parseTerm();
    // right-nest them: \p1.\p2.…body
    for (auto it = params.rbegin(); it != params.rend(); ++it) {
      body = std::make_unique<Abstraction>(*it, std::move(body));
    }
    return body;
  }

  // variable
  if (peek().type == TokenType::VARIABLE) {
    const auto &tok = get();
    return std::make_unique<Variable>(tok.lexeme);
  }

  // parenthesized term
  if (accept(TokenType::LPAREN)) {
    std::unique_ptr<Expr> e = parseTerm();
    expect(TokenType::RPAREN);
    return e;
  }

  throw std::runtime_error("parser: expected \\, variable, or '(' at position "
                         + std::to_string(peek().start));
}

// <application> ::= <simple> { <simple> }
std::unique_ptr<Expr> Parser::parseApplication() {
  std::unique_ptr<Expr> expr = parseSimple();
  while (true) {
    TokenType t = peek().type;
    if (t == TokenType::VARIABLE ||
        t == TokenType::LAMBDA   ||
        t == TokenType::LPAREN)
    {
      std::unique_ptr<Expr> rhs = parseSimple();
      expr = std::make_unique<Application>(std::move(expr), std::move(rhs));
    } else {
      break;
    }
  }
  return expr;
}

// <term> ::= <application>
std::unique_ptr<Expr> Parser::parseTerm() {
  return parseApplication();
}

// parse exactly one term and expect EOC
std::unique_ptr<Expr> Parser::parse() {
  std::unique_ptr<Expr> root = parseTerm();
  expect(TokenType::EOC);
  return root;
}

// <program> ::= { <definition> | <term> EOL } EOC
std::vector<std::unique_ptr<Expr>> Parser::parseProgram() {
  std::vector<std::unique_ptr<Expr>> results;

  while (peek().type != TokenType::EOC) {
    // first try a definition
    if (tryParseDefinition())
      continue;

    // otherwise a bare term ending in EOL
    results.push_back(parseTerm());
    expect(TokenType::EOL);
  }

  expect(TokenType::EOC);
  return results;
}

