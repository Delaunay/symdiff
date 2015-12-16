/*
 *  Symbolic Derivation in C++
 *
 *  Author: Pierre Delaunay
 *
 *  Date: 2015-12-15
 *
 */
#include <iostream>
//#include <memory>

#include "operators.h"
//#include "sym.h"

using namespace std;
//using namespace sym;

//typedef std::shared_ptr<sym::Expr> Sym;

//Sym operator+(Sym& a, Sym b){
//    return Sym(mult(a.get(), b.get()));
//}


int main()
{

//    Sym x = Sym::var("x");

//    Sym f = x * x * x;

//    std::cout << f << std::endl;

//    Sym x = var("x");

//    Sym f = x * x * x;

    // variables
    auto x = sym::var("x");
    auto y = sym::var("y");

    // function
    auto f = sym::mult(sym::mult(x, x), y);

    // Hastable of values
    sym::Variables partial = {{sym::var(x), 3}};
    sym::Variables full = {{sym::var(x), 3}, {sym::var(y), 3}};

    std::cout << " f  : ";   f->print(std::cout) << "\n";
    std::cout << " fx : ";   f->eval(partial)->print(std::cout) << "\n";    // eval return an expression
    std::cout << " fx : " << f->full_eval(full) << "\n\n";                  // full_eval is faster and return a real
                                                                            // require all unkown to be set
    auto dxf = f->derivate(x);

    std::cout << "dxf  : ";   dxf->print(std::cout) << "\n";
    std::cout << "dxfx : ";   dxf->eval(partial)->print(std::cout) << "\n";
    std::cout << "dxfx : " << dxf->full_eval(full) << "\n\n";

    auto dyf = f->derivate(y);

    std::cout << "dyf  : ";   dyf->print(std::cout) << "\n";
    std::cout << "dyfy : ";   dyf->eval(partial)->print(std::cout) << "\n";
    std::cout << "dyfy : " << dyf->full_eval(full) << "\n\n";

    return 0;
}

