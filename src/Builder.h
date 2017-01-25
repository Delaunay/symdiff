#ifndef SYMDIFF_INTERNAL_BUILDER_HEADER_
#define SYMDIFF_INTERNAL_BUILDER_HEADER_

#include "Nodes.h"
#include "Equiv.h"

#include <cmath>

#include <iostream>

// Useful for debugging
// disable basic simplifications
//#define SYMDIFF_NO_SIMP

/*
 *  Note: Switching between Node and NodeImpl is a little messy
 *  Would be nice to have something really well defined
 *  When NodeImpl should be passed and when Node is preferred
 */
namespace symdiff
{

Node minus_one();
Node zero();
Node one();
Node two();

namespace internal {
    const int SCALAR_PREALLOC_START = -100;
    const int SCALAR_PREALLOC_END   =  100;

    inline
    std::vector<Node> _gen_values(int s = SCALAR_PREALLOC_START,
                                  int e = SCALAR_PREALLOC_END){
        std::vector<Node> v(e - s + 1);
        for(int i = 0; i < (e - s) + 1; ++i)
            v[i] = make_value(s + i);
        return v;
    }

    // Pre allocate common values
    inline
    Node values(int i){
        static std::vector<Node> val = _gen_values();
        return val[i - SCALAR_PREALLOC_START];
    }

    // Get pointer to value zero
    inline
    NodeRef zero_ptr(){   static NodeImpl* zp = zero().get();  return zp;  }
    inline
    NodeRef one_ptr() {   static NodeImpl* zp = one().get();   return zp;  }
}

inline Node minus_one(){    return internal::values(-1);   }
inline Node zero()     {    return internal::values( 0);   }
inline Node one()      {    return internal::values( 1);   }
inline Node two()      {    return internal::values( 2);   }

inline bool is_null(Node& ptr)          {    return ptr.get() == internal::zero_ptr();         }
inline bool is_one(Node&  ptr)          {    return ptr.get() == internal::one_ptr();          }

class Operator
{
public:
    // Now that a Value has a type we must allow multiple definition of add in function
    // if the type
    typedef double ValueType;

    // Binary
    static ValueType add (ValueType lhs, ValueType rhs) {   return lhs + rhs; }
    static ValueType mult(ValueType lhs, ValueType rhs) {   return lhs * rhs; }
    static ValueType div (ValueType lhs, ValueType rhs) {   return lhs / rhs; }
    static ValueType sub (ValueType lhs, ValueType rhs) {   return lhs - rhs; }
    static ValueType pow (ValueType lhs, ValueType rhs) {   return ::pow(lhs, rhs); }

    // Unary
    static ValueType inv(ValueType expr) {  return 1.0 / expr; }
    static ValueType neg(ValueType expr) {  return - expr;     }
};

// Hollow class for now
class Builder{
public:

    // Helpers: node builder
    // Those implement basic optimization (constant folding and trivial simplification)
    // You probably always want to use them.

    // Binary
    static Node add (Node lhs, Node rhs);
    static Node mult(Node lhs, Node rhs);
    static Node pow (Node lhs, Node rhs);
    // Abstract
    static Node minus(Node lhs, Node rhs) { return add (lhs, neg(rhs)); }
    static Node div  (Node lhs, Node rhs) { return mult(lhs, inv(rhs)); }

    // Unary
    static Node inv(Node expr);
    static Node neg(Node expr);

    // Leafs
    // Avoid allocating memory for commonly used values (0, 1, etc...)
    static Node value(double val){
        int v = int(val);

        if (v < internal::SCALAR_PREALLOC_START ||
            v > internal::SCALAR_PREALLOC_END)
            return make_value(val);

      return internal::values(v);
    }

    // This one does not do anything special but it is nice to have it for
    // consistency
    static Node placeholder(const std::string& name) { return make_placeholder(name); }
    static Node cond(Node cond, Node tr, Node fl){ return make_cond(cond, tr, fl); }
};

inline
void reorder(Node& lhs, Node& rhs){
    if (lhs->id > rhs->id){
        std::swap(lhs, rhs);
    }
}

inline
Node Builder::add (Node lhs, Node rhs){
    reorder(lhs, rhs);

#ifndef SYMDIFF_NO_SIMP
    // desactivate simplification for patterns
    //if (lhs->is_pattern() || rhs->is_pattern())
    //    return Expression::make<Add>(lhs, rhs);

    // Simplify here
    if (is_null(lhs))  return rhs;
    if (is_null(rhs))  return lhs;

    // NB: it might be better to have a special Addflat
    // which flatten chained addition and collapse them later
    // A Flatten node can be useful for mult too!
    // we could simplify : (x + 2) + 3 easily

    // AddFlatten could greatly reduce graph depth
    // This could also create more balanced tree

    // Worst Unbalanced tree : (a + (b + (c + (d + (e + (f + g) : Depth = 6
    // Best Case             : ((a + b) + c) + (d + (f + g))    : Depth = 3

    // After reflexion an unbalanced tree might generate more cache friendly
    // VM instructions
    // EX: 2 * 2 * 2 * 2
    // Balanced takes less memmory

    /*  Balanced        Unbalanced
     * -----------------------------
     *   push 2         push 2
     *   push 2         push 2
     *   mult           push 2
     *   push 2         push 2
     *   push 2         mult
     *   mult           mult
     *   mult           mult
     **********************************/


    // AddFlatten            : (a + b + c + d + f + g)          : Depth = 1
    // In that case we will have to do 6 add operation as in the worst case
    // but we do not have to carry a graph representation
    // so it should be faster.
    // + now we have each term in a Vector, even if the speed gain is
    // null. It will be easier to transform the graph with a vector

    //      Add Collapse
    // -----------------------
    // /!\ Scalar Node Order

    // (a + a + a + a)
    // (a + (a + (a + a)))
    // (a + (a + (2 * a)))

    //if (equiv(lhs, rhs))
    //    return Mult::make(two(), lhs);

    // This is where ordered term become useful.
    // I don't have to check for all possible cases
    if (rhs->id == NodeID::mult){
        BinaryNode* m = to_binary(rhs);

        if (m->lhs->id == NodeID::value && equiv(m->rhs, lhs)){
            return make_mult(make_value(get_value(m->lhs) + 1), lhs);
        }
    }

    // (a - a)
    // (a + Neg(a))
    if (rhs->id == NodeID::neg){
        UnaryNode* o = to_unary(rhs);

        if (equiv(lhs, o->expr))
            return zero();
    }
#endif
    return make_add(lhs, rhs);
}
inline
Node Builder::pow (Node lhs, Node rhs){
    return make_pow(lhs, rhs);
}
inline
Node Builder::mult (Node lhs, Node rhs){
    reorder(lhs, rhs);

    // desactivate simplification for patterns
    //if (lhs->is_pattern() || rhs->is_pattern())
    //    return Expression::make<Mult>(lhs, rhs);
#ifndef SYMDIFF_NO_SIMP
    // Simplify here
    if (is_null(lhs) || is_null(rhs))
        return zero();

    if (is_one(lhs))  return rhs;
    if (is_one(rhs))  return lhs;

    //      Mult Collapse
    // -----------------------
    // /!\ Scalar Node Order

    // (a * a * a * a)
    // (a * (a * (a * a)))
    // (a * (a * (a ^ 2)))

    /*/ Base case
    if (equiv(lhs, rhs))
        return make_pow(rhs, two());

    if (rhs->type == NodeID::Pow){
        BinaryNode* m = to_binary(rhs);

        if (m->rhs->type == NodeID::scalar && equiv(m->lhs(), lhs)){
            return Pow::make(lhs, make_scalar(get_value(m->rhs()) + 1));
        }
    } //*/

    // (a / a)
    // (a * inv(a))
    if (rhs->id == NodeID::inv){
        UnaryNode* o = to_unary(rhs);

        if (equiv(lhs, o->expr))
            return one();
    }
#endif
    return make_mult(lhs, rhs);
}

//Node Builder::div (Node lhs, Node rhs){
//    return make_div(lhs, rhs);
//}

//Node Builder::sub (Node lhs, Node rhs){
//    return make_sub(lhs, rhs);
//}
inline
Node Builder::inv(Node expr){
    // desactive simplification for patterns
    //if (expr->is_pattern())
    //    return Expression::make<Inverse>(expr);
#ifndef SYMDIFF_NO_SIMP
    // Simplify Here
    if (is_one(expr))
        return one();

    if (expr->id == NodeID::inv){
        UnaryNode* o = to_unary(expr);
        return o->expr;
    }
#endif
    return make_inv(expr);
}
inline
Node Builder::neg(Node expr){
    // desactive simplification for patterns
    //if (expr->is_pattern())
    //    return Expression::make<Opposite>(expr);
#ifndef SYMDIFF_NO_SIMP
    // Simplify Here
    if (is_null(expr))
        return zero();

    if (expr->id == NodeID::neg){
        UnaryNode* o = to_unary(expr);
        return o->expr;
    }
#endif
    return make_neg(expr);
}

/*
SymExpr Exp::make(SymExpr expr){
    // desactive simplification for patterns
    if (expr->is_pattern())
        return Expression::make<Exp>(expr);

    // Simplify Here
    if (expr->is_one())
        return e();

    if (expr->is_null())
        return one();

    if (expr->get_type() == EST_Ln){
        UnaryOperator* o = dynamic_cast<UnaryOperator*>(expr.get());
        return o->expr();
    }

    return Expression::make<Exp>(expr);
}

SymExpr Ln::make(SymExpr expr){
    // desactive simplification for patterns
    if (expr->is_pattern())
        return Expression::make<Ln>(expr);

    // Simplify Here
    if (expr->is_one())
        return zero();

    if (equiv(expr, e()))
        return one();

    if (expr->get_type() == EST_Exp){
        UnaryOperator* o = dynamic_cast<UnaryOperator*>(expr.get());
        return o->expr();
    }

    return Expression::make<Ln>(expr);
}
*/

}

#endif
