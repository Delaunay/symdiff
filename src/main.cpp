#include "smath.h"
#include <iostream>

using namespace symdiff;
using namespace std;

int main(){


/*
    auto x = make_var("x");

    // auto f = x + x + x + x;
    auto f = x - x;

    f.print(cout) << endl; */



    auto x = make_var("x");
    auto y = make_var("y");

    auto f = gaussian_pdf(1, 2);

    f.gen(std::cout, Standard) << endl;
    f.gen(std::cout, Cpp) << endl;
    f.gen(std::cout, Scheme) << endl;
    //f.derivate("x").print(std::cout) << endl;

                // context is created at compile time (Variadic args)
    auto v1 = full_call(f, Arg("x", make_val(0.25)), Arg("y", make_val(0.25)));

                // temporary context
    auto v2 = full_call(f, {{"x", make_val(0.25)}, {"y", make_val(0.25)}});


    Context v = {
        {"x", make_val(0.25)},
        {"y", make_val(0.25)}
    };

    PtrContext p = {
        {SymExpr(x).get(), make_val(0.25)}
    };

    auto v3 = f.partial_eval(p);

    v3.print(cout) << endl;

    cout << v1 << " " << v2 << " " << endl;

    f = (x + 2) + 3;

    f.print(cout) << endl;

    // Subsitute is equivalent to partial eval
    // because of evaluation rule specified inside T::make
    // I should remove them
    auto sub = f.substitute(v);

    sub.print(cout) << endl;
    sub.reduce().print(cout) << endl;

    // same thing available for partial call/partial eval




    /*
    double mean = 0 ;
    double sd = 1;

    Sym m = make_named_val("m", mean);
    Sym s = make_named_val("s", sd);

    auto x = make_var("x");

    auto c = sqrt(Sym::pi() * make_val(2)) * s; */
    //auto u = - (((x - m) / s) ^ 2);

    // Sym f = Sym::inv(constant) * Sym::exp(up);

            /*
    //Sym f = x ^ y;
    //Sym df = f.derivate("x");

    Sym f = gaussian_pdf(x, y);
    Sym df = f.derivate("x");

    cout << " f: ";  f.print(cout) << endl;
    cout << "df: "; df.print(cout) << endl << endl;

    /*  Full Eval
    Context c = {
        {"x", make_val(4)},
        {"y", make_val(3)}
    };

    cout << c << ": " <<  f.full_eval(c) << endl;
    cout << c << ": " << df.full_eval(c) << endl << endl;

    /*  Partial Eval
    Context p = {
        {"x", make_val(4)}
    };

    f.partial_eval(p).print(std::cout) << std::endl;
    df.partial_eval(p).print(std::cout) << std::endl; */

    return 0;
}
