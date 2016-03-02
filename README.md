SymDif
======

Experiment with Symbolic differentiation in C++ and Python.

    * py/ for symbolic dif in python
    * src/ for symbolic dif in C++
    
Python version is a lot more advanced (Garbage collection makes it easier).


# Python
    
    x = Unknown('x')
    y = Unknown('y')

    val = {x: scalar(5)}

    f = x ** 3 - y ** 2    # add(pow(x, y), mult(y, x))
    dfdx = f.derivate(x)

    print(' f   : ', f,    '\tEval: ', f.eval(val))
    print('dfdx : ', dfdx, '\tEval: ', dfdx.eval(val))

    val = {x: scalar(5), y: scalar(2)}
    print(' f   : ', f,    '\tEval: ', f.full_eval(val))
    print('dfdx : ', dfdx, '\tEval: ', dfdx.full_eval(val))
    

# C++


    #include "sym.h"
    #include <iostream>

    using namespace symdif;

    int main(){

        /*  Expr building */
        Sym x = make_var("x");
        Sym y = make_var("y");

        Sym f = x * x * y;

        f.print(std::cout) << std::endl;

        Sym df = f.derivate("x");

        df.print(std::cout) << std::endl;

        /*  Full Eval */
        Context c = {{"x", make_val(4)}, {"y", make_val(3)}};

        std::cout << f.full_eval(c) << std::endl;
        std::cout << df.full_eval(c) << std::endl;

        /*  Partial Eval */
        Context p = {{"x", make_val(4)}};

        f.partial_eval(p).print(std::cout) << std::endl;
        df.partial_eval(p).print(std::cout) << std::endl;

        return 0;
    }
