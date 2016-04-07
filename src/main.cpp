
#include "smath.h"

//#include "sym.h"
#include <iostream>

using namespace symdiff;
using namespace std;

int main(){

    // Expr building

    auto f = gaussian_pdf(0, 1);

    f.print(std::cout) << endl;

    Context arg = {{"x", make_val(0.25)}};

    cout << f.full_eval(arg) << endl;

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
