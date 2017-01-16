#include "Sym.h"
#include <iostream>
#include <cstdlib>

#include "StackVM.h"
#include "RegisterVM.h"

using namespace symdiff;

int main()
{
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

    return 0;
}
