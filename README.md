Symdiff
=======

[![Build Status](https://travis-ci.org/Delaunay/symdiff.svg?branch=master)](https://travis-ci.org/Delaunay/symdiff)

Symbolic differentiation in C++, Python and C. All of them are standalone implementations.
This is not intended to be an efficient Symbolic library but rather a
relatively simple example of how it could be implemented.

* py/ for symbolic diff in python
* src/ for symbolic diff in C++
* csymdiff/ for symbolic diff in C

If you are using MinGW. You need to disable pthreads on windows
(`cmake -Dgtest_disable_pthreads=ON`)

# Requirement

* Python: None
* C : None
* C++ : cmake, LLVM is optional

Compiled with MSVC2015 (64bit) and LLVM-3.8.1 on Windows 10

# Python

    x = Unknown('x')
    y = Unknown('y')

    env = {x: scalar(5), y: scalar(2)}

    f = x ** 3 - y ** 2   
	print(f)
	
    dfdx = f.derivate(x)
	dfdy = f.derivate(y)
	
	val = f.eval(env)

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
        Context env = {{"x", make_val(4)}, {"y", make_val(3)}};

        std::cout << f.full_eval(c) << std::endl;
        std::cout << df.full_eval(c) << std::endl;

        /*  Partial Eval */
        Context p = {{"x", make_val(4)}};

        f.partial_eval(p).print(std::cout) << std::endl;
        df.partial_eval(p).print(std::cout) << std::endl;

        return 0;
    }

# C

	int main ()
	{
	    SymExpr x = sym_placeholder("x");
	    SymExpr expr = sym_mult(x, x);

	    sym_print(expr);	 printf("\n");

	    SymExpr df = sym_deriv("x", expr);

	    sym_print(df);	 	 printf("\n");

	    sym_free(df);
	    sym_free(expr);
	    sym_free(x);

	    return 0;
	}

# Features

* Pretty Printing (C++, Python, C)
* Basic Optimization (Constant folding, Mult by zero/one, etc...) (C++, Python)
* LLVM-IR code gen (if LLVM is installed) (C++)

	The expression `10 + y + 3 * x` becomes: (you can choose the function's name)

        define double @my_fun(double %y, double %x) {
        fun_body:
           %0 = fmul double 3.000000e+00, %x
           %1 = fadd double %y, %0
           %2 = fadd double 1.000000e+01, %1
           ret double %2
        }
	

* `StrContext` (C++, Python) and `PtrContext` (C++)
    * StrContext is used to substitute using variable's name (Only placeholders can be subs)
* Pattern Matching (C++)
* Stack Based VM (C++)

         (if (+ -1 (- 3)) (+ x 2) (+ y 3))

         0 push -1
         1 push 3
         2 neg
         3 add
         4 branch 5
         5 lookup y
         6 push 3
         7 add
         8 push 1
         9 branch 3
        10 lookup x
        11 push 2
        12 add

* Register Based VM (C++)

         (if (+ -1 (- 3)) (+ x 2) (+ y 3))

         0 load -1 => 0
         1 load 3 => 1
         2 neg 1, 0, 1
         3 add 0, 1, 0
         4 branch 0 ? 5
         5 lookup y => 0
         6 load 3 => 1
         7 add 0, 1, 0
         8 load 1 => 1
         9 branch 1 ? 3
        10 lookup x => 0
        11 load 2 => 1
        12 add 0, 1, 0

# TODO

* factorize/expand
* Would be nice to be able to parse a string

        f = expr
        std::stringstream ss, ff;
        f.print(ss);
        std::string fstr = ss.str();

        g = read(fstr);
        f.print(ff);
        std::string gstr = ff.str();

        if (gstr == fstr)
            success ()


# IDEAS

* Type system
* Function Call
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
    * Placeholder could be generalized
    * ConstTensor

* Cached eval result

* `find_node(mult(2, add(3, 4))`
* `find_pattern(mult(any, add(any, any)))`    // Any is a special node that can not be evaluated
* `find_all_node`
* `find_all_pattern`
* `replace(Pattern, Sym by)`
* `replace(node, Sym by)`


# Branching (VMs)

Branching is only done 'forward' (skip instructions) you can rollback to previous
instructions. So you cannot create loops using the provided branch operation.

