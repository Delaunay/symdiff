#ifndef SYMDIF_INTERNAL_EXPR_HEADER
#define SYMDIF_INTERNAL_EXPR_HEADER

#include "Context.h"
#include <iostream>

// TODO
// return a set of Placeholder variable inside a given expression
// #include <unordered_set>

namespace symdiff{

enum OutputType
{
    Standard, // Default, kinda standard looking output
    Cpp,      // C++
    Scheme,   // Lisp/Scheme like  syntax

    // Latex
    // Python
};

namespace internal{

// Custom RTTI
// Those are not directly related with operator precedence
// But they respect it
// We can use get_type() to determine if a Expr is a Leaf/Binary/Unary expr
enum ExprSubType
{
    // Leafs
    LEAF_SUBTYPE    = 100,
    EST_Scalar,
    EST_Placeholder,

    // Nodes::Binary
    BINARY_SUBTYPE  = 200,
    EST_Add,
    EST_Mult,
    EST_Pow,

    // Nodes::Unary
    UNARY_SUBTYPE   = 300,
    EST_Sub,
    EST_Neg,
    EST_Exp,
    EST_Ln,

    // For printing sake Inv must be last
    // (/ a) b => b (/ a)
    EST_Inv,

    PATTERN_SUBTYPE = 400,
};

#define IS_BINARY(type) (type > internal::BINARY_SUBTYPE) && (type < internal::UNARY_SUBTYPE)
#define IS_UNARY(type) (type > internal::UNARY_SUBTYPE)
#define IS_LEAF(type) (type < internal::BINARY_SUBTYPE)

std::string to_string(ExprSubType t);

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

    virtual std::ostream& gen(std::ostream& out, OutputType t) = 0;

    // used for simplification
    virtual bool is_nul()    {  return false;   }
    virtual bool is_one()    {  return false;   }
    virtual bool is_scalar() {  return false;   }
    virtual bool is_leaf()   {  return false;   }
    virtual bool is_binary() {  return false;   }
    virtual bool is_pattern(){  return false;   }
    virtual bool parens()    {  return false;   }   // For pretty printing

    // return max Tree depth
    virtual int depth(int i = 0) = 0;

    // basic graph transformation
    virtual double full_eval(Context& c)  = 0;
    virtual SymExpr partial_eval(Context& c) = 0;

    virtual double full_eval(PtrContext& c)  = 0;
    virtual SymExpr partial_eval(PtrContext& c) = 0;

    // virtual SymExpr derivate(const SymExpr& name) = 0;
    virtual SymExpr derivate(const std::string& name) = 0;

    virtual SymExpr substitute(Context& x) = 0;
    virtual SymExpr reduce() {  Context c; return partial_eval(c);  }

    // implicit conversion to ExprSubType so we can switch over expression
    // not used
    virtual ExprSubType get_type() const = 0;
    virtual std::string to_string(){ return internal::to_string(this->get_type()); }

    /* Graph utilities */
    // sym_equal test only if a node b is of the same 'type'
    // Binary/Unary EST_Type == EST_type
    // Placeholder  _name == _name
    // Scalar       _value == _value
    virtual bool sym_equal(SymExpr& ) { return false;   }

    // Similar to sym equal but allow for special Pattern Node
    virtual bool pattern_equal(SymExpr& a) { return sym_equal(a);   }
    virtual bool equal(SymExpr& a) = 0;

private:
};



} // internal namespace
} // symdiff namespace

#endif
