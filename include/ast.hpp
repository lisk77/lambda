#ifndef AST_HPP
#define AST_HPP

#include <memory>
#include <string>

// ast stuff lul
struct Expr {
    virtual ~Expr() = default;
    virtual std::unique_ptr<Expr> clone() const = 0;
};

// represents a simple symbol like "x" or even "thisIsACoolFunctionTrustme100"
// (ikik the true lambda calculus professionals would now scream at me and say
// "lambda calculus has only one letter variables!", to which id say ¯\_(ツ)_/¯
struct Variable : Expr {
    std::string name;

    Variable(std::string n) : name(std::move(n)) {}
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<Variable>(name);
    }
};

// represents a function itself like \ x . x
struct Abstraction : Expr {
    std::string param;
    std::unique_ptr<Expr> body;

    Abstraction(std::string p, std::unique_ptr<Expr> b) : param(std::move(p)), body(std::move(b)) {}
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<Abstraction>(param, body->clone());
    }
};

// wouldnt be turing complete if we couldnt apply to these functions innit
// (\ x . x) a would be such an application
// note that parenthesis are your savior because (\ x y . x) (\ x . x) b and
// (\ x y . x) ((\ x . x) b ) are not the same thing
struct Application : Expr {
    std::unique_ptr<Expr> left, right;

    Application(std::unique_ptr<Expr> l, std::unique_ptr<Expr> r) : left(std::move(l)), right(std::move(r)) {}
    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<Application>(left->clone(), right->clone());
    }
};

#endif // AST_HPP
