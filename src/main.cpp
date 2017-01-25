#include "Sym.h"
#include <iostream>
#include <cstdlib>

#include "StackVM.h"
#include "RegisterVM.h"

#include "PrettyPrint.h"

using namespace symdiff;

int main()
{
    /*
    auto x = make_var("x");
    auto f = (x + 2) * (x + 3);
    auto df = f.derivate("x");

    std::cout <<  f << std::endl;
    std::cout << df << std::endl;

    NameContext env {
        {"x", make_var(2)}
    };

    std::cout << "ctx: " << env << std::endl;
    std::cout << "Val: " << f.full_eval(env) << std::endl;
    std::cout << "Val: " << f.rvm_eval (env) << std::endl;
    std::cout << "Val: " << f.svm_eval (env) << std::endl;

    // Pattern matcher
    auto y = make_var("y");
    auto pa = make_any();
    auto pb = make_any();
    auto pattern = pa * pb;
    auto expr = (x + 2) * (y + 3);

    std::cout << pattern << std::endl;
    std::cout << expr.match(pattern) << std::endl;

    PrettyPrint::run(std::cout, to_unary(pa.to_node())->expr) << std::endl;
    //*/

    auto x = make_var("x");
    auto y = make_var("y");
    auto g = cond(make_var(-1) - 3, x + 2, y + 3);
    //auto g = cond(make_var(-1), x, y);
    auto ng = g.to_node();

    PolishPrint::run(std::cout, ng) << std::endl;

    auto sinsts = StackGenerator::run(ng);
    auto rinsts = RegisterGenerator::run(ng);

    //print(std::cout, sinsts) << std::endl;
    print(std::cout, rinsts) << std::endl;

    NameContext env {
        {"x", make_var(4)},
        {"y", make_var(5)}
    };

    //std::cout << "r: " << StackVM::run(env, insts) << std::endl;
    std::cout << "r: " << RegisterVM::run(env, rinsts) << std::endl;

    return 0;
}
