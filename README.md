Symdiff
=======

Symbolic differentiation in C++ and Python. Both are standalone implementations.
This is not intended to be an efficient Symbolic library but rather a
relatively simple example of how it could be implemented.


* py/ for symbolic diff in python
* src/ for symbolic diff in C++

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

# TODO

* factorize/expand
* tests
    Would be nice to be able to parse a string
        f = expr
        std::stringstream ss, ff;
        f.print(ss);
        std::string fstr = ss.str();

        g = read(fstr);
        f.print(ff);
        std::string gstr = ff.str();

        if (gstr == fstr)
            success ()

# IDEA

* Type system
* Check if Graph can be 'easily' balanced
* Check if we can use threads to compute two different branches
* Check shared_ptr and thread
    Thread share pointers put they cannot deallocate those.
    Only temporary pointers will be deallocated.


     level 1 - Threading

     Thread 1 | Thread 2
              *                 level 0 - 1 Possible Thread
           /  |  \
          /   |   \
         +    |    +            level 1 - 2 possible Thread
       /   \  |  /   \
      a     b | c     d         level 2 - 4 Possible Thread
              |

    We cannot spawn thread at each nodes there are too many of them.
    but we could spam per level.

    Seek the level best fitting our threading capability.

    NB: Number of possible thread per level is not necessarily even. (Unary Nodes)


* Add Tensor
    Placeholder could be generalized
    ConstTensor

* Add Flow control ?

* Cached eval result







