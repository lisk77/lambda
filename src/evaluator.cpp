#include "evaluator.hpp"
#include "ast.hpp"
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_set>

//  TODO: optimize that later lol

static std::unique_ptr<Expr> clone(const std::unique_ptr<Expr>& e) {
    if (auto v = dynamic_cast<Variable*>(e.get()))
        return std::make_unique<Variable>(v->name);
    if (auto ab = dynamic_cast<Abstraction*>(e.get()))
        return std::make_unique<Abstraction>(ab->param, clone(ab->body));
    if (auto ap = dynamic_cast<Application*>(e.get()))
        return std::make_unique<Application>(clone(ap->left), clone(ap->right));
    throw std::runtime_error("clone: unknown expression type");
}

// alpha conversion because lambda calc is all global scope
static std::string newVar(const std::string& baseVar, const std::unordered_set<std::string>& avoid) {
    std::string var = baseVar + "'";

    while (avoid.count(var)) {
        var += "'";
    }

    return var;
}

// gives you all free variables in the expression
static std::unordered_set<std::string> freeVars(const std::unique_ptr<Expr>& expr) {
    std::unordered_set<std::string> result;

    if (auto v = dynamic_cast<Variable*>(expr.get())) {
        result.insert(v->name);
    }

    else if (auto ab = dynamic_cast<Abstraction*>(expr.get())) {
        auto bodyFVs = freeVars(ab->body);
        bodyFVs.erase(ab->param);
        result = bodyFVs;
    }

    else if (auto ap = dynamic_cast<Application*>(expr.get())) {
        auto leftFVs = freeVars(ap->left);
        auto rightFVs = freeVars(ap->right);
        result.insert(leftFVs.begin(), leftFVs.end());
        result.insert(rightFVs.begin(), rightFVs.end());
    }

    return result;
}

// beta reduction because we obviously want to apply parameters to functions
std::unique_ptr<Expr> substitute(const std::string& var, const std::unique_ptr<Expr>& val, const std::unique_ptr<Expr>& expr) {
    if (auto v = dynamic_cast<Variable*>(expr.get())) {
        if (v->name == var) {
            return clone(val);
        }

        else {
            return clone(expr);
        }
    }

    if (auto ab = dynamic_cast<Abstraction*>(expr.get())) {
        if (ab->param == var) {
            return clone(expr);
        }

        auto valFVs = freeVars(val);
        if (valFVs.count(ab->param)) {
            auto exprFVs = freeVars(expr);
            auto allFVs = valFVs;
            allFVs.insert(exprFVs.begin(), exprFVs.end());
            allFVs.insert(var);
            allFVs.erase(ab->param);

            std::string newParam = newVar(ab->param, allFVs);
            std::unique_ptr<Expr> renamedBody = substitute(ab->param, std::make_unique<Variable>(newParam), ab->body);
            std::unique_ptr<Expr> newBody = substitute(var, val, renamedBody);
            return std::make_unique<Abstraction>(newParam, std::move(newBody));
        }

        else {
            std::unique_ptr<Expr> newBody = substitute(var, val, ab->body);
            return std::make_unique<Abstraction>(ab->param, std::move(newBody));
        }
    }

    if (auto ap = dynamic_cast<Application*>(expr.get())) {
        std::unique_ptr<Expr> newLeft = substitute(var, val, ap->left);
        std::unique_ptr<Expr> newRight = substitute(var, val, ap->right);
        return std::make_unique<Application>(std::move(newLeft), std::move(newRight));
    }

    throw std::runtime_error("substitute: unknown expression type");
}

// idk about you but i would like my expressions to be simpler than just "put in for x and done"
std::unique_ptr<Expr> normalize(const std::unique_ptr<Expr>& expr, const std::unordered_map<std::string, std::unique_ptr<Expr>>& env) {
    if (auto v = dynamic_cast<Variable*>(expr.get())) {
        auto it = env.find(v->name);
        if (it != env.end()) {
            return normalize(it->second, env);
        }
        return clone(expr);
    }

    if (auto ab = dynamic_cast<Abstraction*>(expr.get())) {
        std::unique_ptr<Expr> normalizedBody = normalize(ab->body, env);
        return std::make_unique<Abstraction>(ab->param, std::move(normalizedBody));
    }

    if (auto ap = dynamic_cast<Application*>(expr.get())) {
        std::unique_ptr<Expr> func = normalize(ap->left, env);
        if (auto ab = dynamic_cast<Abstraction*>(func.get())) {
            std::unique_ptr<Expr> result = substitute(ab->param, ap->right, ab->body);
            return normalize(result, env);
        }

        else {
            std::unique_ptr<Expr> arg = normalize(ap->right, env);
            return std::make_unique<Application>(std::move(func), std::move(arg));
        }
    }

    throw std::runtime_error("normalize: unknown expression type");
}

// i think that is self explanatory
std::unique_ptr<Expr> evaluateMain(const std::unordered_map<std::string, std::unique_ptr<Expr>>& defs) {
    auto it = defs.find("main");
    if (it == defs.end()) throw std::runtime_error("evaluate: no main function found in file");
    return normalize(it->second, defs);
}
