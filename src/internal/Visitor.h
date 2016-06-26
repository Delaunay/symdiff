#ifndef SYMDIF_INTERNAL_VISITOR_HEADER
#define SYMDIF_INTERNAL_VISITOR_HEADER

#include <utility>      // std::forward


namespace symdiff {
namespace internal{

class Add;
class Mult;
class Pow;
class Sub;
class Neg;
class Ln;
class Exp;
class Inverse;

//class ScalarDouble;
class Placeholder;

class BinaryOperator;
class UnaryOperator;


class Visitor
{
public:
    virtual ~Visitor() {}

#define make_member(fun_name, node_name) virtual void fun_name(const node_name*)   {           }

    // Generic Nodes
     make_member(binary, BinaryOperator)
     make_member(unary,  UnaryOperator)
//     make_member(leaf,   Leaf)

    /*
     *  Should check the possibility to do Definitions.cpp in a Definition.def
     *  and use Definition.def as a XMACRO file
     *  to generate this automatically
     */

    // Specific Nodes
    make_member(add,  Add)
    make_member(mult, Mult)
    make_member(pow,  Pow)
    make_member(sub,  Sub)
    make_member(neg,  Neg)
    make_member(ln,   Ln)
    make_member(exp,  Exp)
    make_member(inv,  Inverse)

    make_member(scalar,       ScalarDouble)
    make_member(placeholder,  Placeholder)

#undef make_member

private:
    void visit() {  }

};

}}

#endif
