#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include "ast.hpp"
#include <unordered_map>
#include <string>
#include <stdexcept>

std::unique_ptr<Expr> normalize(const std::unique_ptr<Expr>&, const std::unordered_map<std::string, std::unique_ptr<Expr>>&);
std::unique_ptr<Expr> substitute(const std::string&, const std::unique_ptr<Expr>&, const std::unique_ptr<Expr>&);
std::unique_ptr<Expr> evaluate(const std::unique_ptr<Expr>&, const std::unordered_map<std::string, std::unique_ptr<Expr>>&);
std::unique_ptr<Expr> evaluateMain(const std::unordered_map<std::string, std::unique_ptr<Expr>>&);

#endif // EVALUATOR_HPP
