#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "lexer.hpp"
#include "parser.hpp"
#include "evaluator.hpp"
#include "print.cpp"

int main() {
    // get that stream
    std::ostringstream ss;
    ss << std::cin.rdbuf();
    const std::string src = ss.str();

    // chop it up nicely
    Lexer lex(src);
    auto tokens = lex.tokenize();

    // make sense of the chopping and scream at people if 
    // they are in the wrong order or smth
    Parser parser(tokens);
    auto trees = parser.parseProgram();

    // try to print the evaluation
    try {
        std::unique_ptr<Expr> result = evaluateMain(parser.definitions());
        std::cout << toString(result) << "\n";
    }
    catch (const std::exception &e) {
        // eh.
        std::cerr << "evaluation: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

