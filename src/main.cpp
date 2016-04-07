
#include "sym.h"
#include <iostream>

using namespace symdiff;

int main(){

    /*  Expr building */
    Sym x = make_var("x");
    Sym y = make_var("y");

    Sym f = x * x * y;

    f.print(std::cout) << std::endl;

    Sym df = f.derivate("x");

    df.print(std::cout) << std::endl;

    /*  Full Eval */
    Context c = {
        {"x", make_val(4)},
        {"y", make_val(3)}
    };

    std::cout << f.full_eval(c) << std::endl;
    std::cout << df.full_eval(c) << std::endl;

    /*  Partial Eval */
    Context p = {
        {"x", make_val(4)}
    };

    f.partial_eval(p).print(std::cout) << std::endl;
    df.partial_eval(p).print(std::cout) << std::endl;

    return 0;
}
