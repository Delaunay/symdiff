#ifndef SYMDIF_EXPR_HEADER
#define SYMDIF_EXPR_HEADER

#include "Context.h"

namespace symdiff{

namespace internal{

// Custom RTTI
// Those are not directly related with operator precedence
enum ExprSubType
{
    // Leafs
    EST_Scalar      = 100,
    EST_Placeholder,

    // Nodes::Binary
    EST_Add         = 200,
    EST_Mult        = 300,
    EST_Pow         = 400,

    // Nodes::Unary
    EST_Sub         = 1000,
    EST_Neg,
    EST_Inv,
    EST_Exp,
    EST_Ln,
};

class Expression{
public:
    virtual ~Expression() {}

    /* Print representation */
    virtual std::ostream& print(std::ostream& out) = 0;

    // Special constructor
    // This is the constructor that should be used internally
    // it makes simplification. It does not always return the
    // type of the constructed object (i.e Add(3, Add(3, 3)) -> Mult(3, 3)
    // that is why it cannot be implemented in the usual constructor
    // T(arg1, arg1) => Expression::make(arg1, arg2)
    // this is a default and generic version of the function
    // that can be used as a base case (no simplification)
    template<typename T, typename... Args>
    static SymExpr make(Args&&... args){
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    // This is not a mathematical < operator but rather
    // a function that force operation order inside an expression
    // i.e  (2 * a) and (a * 2) are the same and should be represented the same way
    // instead of having to check for both we force
    // Scalar to be first (order is given by ExprSubType ID)
    // i.e if you input (a * 2) symdiff will reorder terms as (2 * a)
    // reordering is only done for associative nodes (duh)

    // Reordering terms is useful for two main reasons:
    //      - Only one term need to be checked during simplification
    //      - The fringe problem with associative nodes become trivial
    //              (used to identify node equality)

    // actually, reorder do no use this anymore
    bool operator < (const Expression& a) {
        return this->get_type() < a.get_type();
    }

    // used for simplification
    virtual bool is_nul()    {  return false;   }
    virtual bool is_one()    {  return false;   }
    virtual bool is_scalar() {  return false;   }
    virtual bool is_leaf()   {  return false;   }
    virtual bool parens()    {  return false;   }   // For more pretty printing

    // basic graph transformation
    virtual double full_eval(Context& c)  = 0;
    virtual SymExpr partial_eval(Context& c) = 0;
    virtual SymExpr derivate(const std::string& name) = 0;

    virtual SymExpr substitute(Context& x) = 0;
    virtual SymExpr reduce() {  Context c; return partial_eval(c);  }

    // implicit conversion to ExprSubType so we can switch over expression
    // not used
    virtual ExprSubType get_type() const = 0;

private:
};



} // internal namespace
} // symdiff namespace

#endif
