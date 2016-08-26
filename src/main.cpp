#include "Builder.h"
#include "PrettyPrint.h"
#include "FullEval.h"
#include "PartialEval.h"
#include "Derivate.h"

#include <iostream>

using namespace symdiff;

int main()
{
    auto x = Builder::placeholder("x");
    auto f = Builder::mult(Builder::add(one(), two()), x);

    PrettyPrint::run(std::cout, f);

    NameContext env {
        {"x", Builder::value(2)}
    };

    std::cout << "\n ctx: " << env << std::endl;

    std::cout << "Val: " << FullEval::run(env, f) << std::endl;

    auto pf = PartialEval::run(f);

    std::cout << "Partial: "; PrettyPrint::run(std::cout, pf); std::cout << "\n";

    auto df = Derivate::run("x", pf);

    std::cout << "Derivate: "; PrettyPrint::run(std::cout, df); std::cout << "\n";

    return 0;
}
