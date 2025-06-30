#include "ast.hpp"
#include <string>
#include <sstream>
#include <vector>

// thank you chatgpt lol

// Forward‐declare
static void _print(const Expr* e, std::ostream& out);

// Public API: return string
inline std::string toString(const std::unique_ptr<Expr>& e) {
    std::ostringstream oss;
    _print(e.get(), oss);
    return oss.str();
}

// Internal recursive printer
static void _print(const Expr* e, std::ostream& out) {
    // 1) Abstraction(s): collect all params
    if (auto ab = dynamic_cast<const Abstraction*>(e)) {
        std::vector<std::string> params;
        const Expr* body = ab;
        while (auto a2 = dynamic_cast<const Abstraction*>(body)) {
            params.push_back(a2->param);
            body = a2->body.get();
        }
        // print "\x y z. "
        out << '\\' << params[0];
        for (size_t i = 1; i < params.size(); ++i) {
            out << ' ' << params[i];
        }
        out << ". ";
        // then the body
        _print(body, out);
        return;
    }

    // 2) Variable
    if (auto v = dynamic_cast<const Variable*>(e)) {
        out << v->name;
        return;
    }

    // 3) Application
    if (auto ap = dynamic_cast<const Application*>(e)) {
        out << '(';
        _print(ap->left.get(), out);
        out << ' ';
        _print(ap->right.get(), out);
        out << ')';
        return;
    }

    // fallback
    out << "print: unknown expression";
}

