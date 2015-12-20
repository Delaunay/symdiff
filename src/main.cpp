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
//    const sym::TreeBuilder x = sym::vvar("x");

//    auto f = x * x * x;

//    f.print();

//    cout << "\n";

//    f.derivate(x).print();

//    cout << "\n";

//    Sym x = Sym::var("x");

//    Sym f = x * x * x;

//    std::cout << f << std::endl;

//    Sym x = var("x");

//    Sym f = x * x * x;

    sym::TreeBuilder tb = sym::function();

    // variables
    sym::Expr x = sym::var("x");
    sym::Expr y = sym::var("y");

    // function
    sym::Expr f = sym::mult(sym::mult(x, x), x); // sym::mult(sym::pow(x, 3), y);//

    // Hastable of values
//    sym::Variables partial = {{sym::var(x), 4}};
//    sym::Variables full = {{sym::var(x), 4}, {sym::var(y), 4}};

    std::cout << " f  : ";   f->print(std::cout) << "\n";
//    std::cout << " fx : ";   f->eval(tb, partial)->print(std::cout) << "\n";    // eval return an expression
//    std::cout << " fx : " << f->full_eval(full) << "\n\n";                  // full_eval is faster and return a real
                                                                            // require all unkown to be set
    auto dxf = f->derivate(tb, x);

    std::cout << "dxf  : ";   dxf->print(std::cout) << "\n";
//    std::cout << "dxfx : ";   dxf->eval(tb, partial)->print(std::cout) << "\n";
//    std::cout << "dxfx : " << dxf->full_eval(full) << "\n\n";

    auto dyf = f->derivate(tb, y);

    std::cout << "dyf  : ";   dyf->print(std::cout) << "\n";
//    std::cout << "dyfy : ";   dyf->eval(tb, partial)->print(std::cout) << "\n";
//    std::cout << "dyfy : " << dyf->full_eval(full) << "\n\n";

    return 0;
}

