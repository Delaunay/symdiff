#ifndef SYMDIF_EXPR_HEADER
#define SYMDIF_EXPR_HEADER

#include "Context.h"

namespace symdiff{

namespace internal{

enum ExprSubType
{
    // Leafs
    EST_Scalar,
    EST_Placeholder,

    // Nodes
    EST_Add,
    EST_Sub,
    EST_Mult,
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
    bool operator < (const Expression& a) {
        return ExprSubType (*this) < ExprSubType (a);
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

    // implicit conversion to ExprSubType so we can switch over expression
    // not used
    virtual operator ExprSubType () const = 0;
private:
};



} // internal namespace
} // symdiff namespace

#endif
