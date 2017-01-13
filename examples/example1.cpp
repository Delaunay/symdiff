#include "Builder.h"
#include "PrettyPrint.h"
#include "FullEval.h"
#include "PartialEval.h"
#include "Derivate.h"

#include "StackVM.h"

#include <iostream>

using namespace symdiff;

int main()
{
    auto x = Builder::placeholder("x1");
    auto f = Builder::mult(Builder::add(one(), two()), x);

    PrettyPrint::run(std::cout, f);

    NameContext env {
        {"x1", Builder::value(2)}
    };

    std::cout << "\n ctx: " << env << std::endl;

    std::cout << "Val: " << FullEval::run(env, f) << std::endl;

    auto pf = PartialEval::run(f);

    std::cout << "Partial: "; PrettyPrint::run(std::cout, pf); std::cout << "\n";

    auto df = Derivate::run("x1", pf);

    std::cout << "Derivate: "; PrettyPrint::run(std::cout, df); std::cout << "\n";

    auto g = Builder::add(Builder::add(Builder::placeholder("x"), Builder::mult(Builder::placeholder("y"), Builder::placeholder("z"))), Builder::placeholder("u"));

    PrettyPrint::run(std::cout, g) << "\n";

    std::cout << "Stack Generator\n";
    auto s = StackGenerator::run( g);

    print(std::cout, s);

    NameContext env2 {
        {"x", Builder::value(2)},
        {"y", Builder::value(3)},
        {"z", Builder::value(4)},
        {"u", Builder::value(5)}
    };

    std::cout << "eval: " << StackVM::run(env2, s) << std::endl;
    std::cout << "eval: " << FullEval::run(env2, g) << std::endl;

    return 0;
}
