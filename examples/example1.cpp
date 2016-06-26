//#include "smath.h"

#include "internal/Nodes.h"
#include <iostream>
#include <functional>

using namespace symdiff;
using namespace std;

struct PrettyPrintVisitor: public symdiff::internal::Visitor
{
    typedef std::function<void(Visitor&, internal::NodeImpl*, std::ostream&)> function_type;

    PrettyPrintVisitor(std::ostream& out, Node n):
        out(out)
    {
        vtable[int(TypeID::add)] = print_add;
        vtable[int(TypeID::scalar)] = print_scalar;

        dispatch(n.get());
    }

    virtual void dispatch(internal::NodeImpl* n){ return vtable[int(n->type)](*this, n, out);}

    // does not seem possible to remove the first virtual function call
    static void print_add(Visitor& self, internal::NodeImpl* n, std::ostream& out){
        BinaryNode* b = static_cast<BinaryNode*>(n);
        self.dispatch(b->lhs.get()); // lhs.get()->visit(*this);
            out << " + ";
        self.dispatch(b->rhs.get());
    }

    static void print_scalar(Visitor& self, internal::NodeImpl* n, std::ostream& out){
        Scalar* s = static_cast<Scalar*>(n);
        out << s->value;
    }

    function_type vtable[int(TypeID::Size)];
    std::ostream& out;
};



int main(){

    auto val = make_scalar(2);
    auto expr = make_add(val, val);

    PrettyPrintVisitor(std::cout, expr);

    std::cout << std::endl;


//    auto x = make_var("x");

//    auto f = exp(exp(ln(x * x + ln(x - one()) - ln(one() / ln(x)))));

//    f.print(std::cout) << std::endl;

//    auto df = f.derivate("x");

//    df.reduce().print(std::cout) << std::endl;

/*
    auto x = make_var("x");

    // auto f = x + x + x + x;
    auto f = x - x;

    f.print(cout) << endl; */


/*
    auto x = make_var("x");
    auto y = make_var("y");

    auto f = gaussian_pdf(1, 2);

    f.gen(std::cout, Standard) << endl;
    f.gen(std::cout, Cpp) << endl;
    f.gen(std::cout, Scheme) << endl;

    std::cout << f.depth() << std::endl;

    auto depth = f.two_depth();

    std::cout << depth.first << " " << depth.second << endl;

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
