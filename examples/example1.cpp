//#include "smath.h"

#include "internal/Nodes.h"
#include <iostream>
#include <vector>
#include <functional>

using namespace symdiff;
using namespace std;

struct PrettyPrint: public symdiff::internal::Visitor
{
    typedef std::function<void(PrettyPrint*, internal::NodeImpl*)> function_type;

    PrettyPrint(std::ostream& out):
        out(out)
    {
        #define SYMDIFF_NODES_DEFINITIONS
            #define DEFINE_LEAF_NODE(__type__, __str__, __repr__)  \
                    _vtable[int(TypeID::__str__)] = &PrettyPrint::__str__;

            #define DEFINE_UNARY_NODE(__type__, __str__, __repr__) \
                    _vtable[int(TypeID::__str__)] = &PrettyPrint::unary_node;

            #define DEFINE_BINARY_NODE(__type__, __str__, __repr__)\
                    _vtable[int(TypeID::__str__)] = &PrettyPrint::binary_node;

            #include "../src/internal/Nodes.def"
        #undef SYMDIFF_NODES_DEFINITIONS
    }

    static void run(std::ostream& out, Node n){
        return PrettyPrint(out).dispatch(n.get());
    }

    void dispatch(internal::NodeImpl* n) final {
        return _vtable[int(n->type)](this, n);
    }

    // Binary node printing
    void binary_node(internal::NodeImpl* n){
        BinaryNode* b = static_cast<BinaryNode*>(n);
        out << "(";
            this->dispatch(b->lhs.get());
                out << " " + _operator[int(n->type)] + " ";
            this->dispatch(b->rhs.get());
        out << ")";
    }

    // Unary Node printing
    void unary_node(internal::NodeImpl* n){
        UnaryNode* u = static_cast<UnaryNode*>(n);
        out << _operator[int(n->type)] << "(";
            this->dispatch(u->expr.get());
        out << ")";
    }

    void scalar(internal::NodeImpl* n){
        Scalar* s = static_cast<Scalar*>(n);
        out << s->value;
    }

    void placeholder(internal::NodeImpl* n){
        Placeholder* s = static_cast<Placeholder*>(n);
        out << s->name;
    }

    std::ostream& out;
    function_type _vtable[int(TypeID::Size)];

    const std::vector<string> _operator{
        #define SYMDIFF_NODES_DEFINITIONS
            #define DEFINE_LEAF_NODE(__type__, __str__, __repr__) #__repr__,
            #define DEFINE_UNARY_NODE(__type__, __str__, __repr__) #__repr__,
            #define DEFINE_BINARY_NODE(__type__, __str__, __repr__) #__repr__,

            #include "../src/internal/Nodes.def"
        #undef SYMDIFF_NODES_DEFINITIONS
    };
};



int main(){

    auto val = make_scalar(2);
    auto expr = make_add(val, val);

    PrettyPrint::run(std::cout, expr);

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
