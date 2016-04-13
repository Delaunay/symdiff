
#include <iostream>

#include "smath.h"

using namespace symdiff;
using namespace std;

int main(){

    Sym x = make_var("x");

    Sym f = (x + 1) * (x - 1);

    Sym p1 = mult(  leaf(), binary());
    Sym p2 = mult( unary(), binary());
    Sym p3 = mult(   any(), binary());
    Sym p4 = mult(binary(), binary());

    cout << "Expr matched: "; f.print(cout) << endl;

        cout << "       Pattern                Match?" << endl;
        cout << "     "; p1.print(cout) << "     \t" << f.equal(p1) << endl ;
        cout << "    "; p2.print(cout) << "     \t" << f.equal(p2)<< endl;
        cout << "    "; p3.print(cout) << "     \t" << f.equal(p3)<< endl;
        cout << "    "; p4.print(cout) << "     \t" << f.equal(p4)<< endl;


//    std::cout << Pa<< f.equal(mult(unary(), binary())) << std::endl;
//    std::cout << f.equal(mult(leaf(), binary())) << std::endl;
//    std::cout << f.equal(mult(any(), binary())) << std::endl;
//    std::cout << f.equal(mult(binary(), binary())) << std::endl;


    return 0;
}
