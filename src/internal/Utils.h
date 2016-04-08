#ifndef SYMDIF_UTILS_HEADER
#define SYMDIF_UTILS_HEADER
/*
 *  Descriptions:
 *    - Define Abstract Unary and Binary Nodes
 *    - Define Helper Macro that facilitate adding nodes
 */

#include "Scalar.h"
#include <functional>

namespace symdiff{
namespace internal{

// reorder is a conditional swap
void reorder(SymExpr& a, SymExpr& b);

// Memory equal     (same ptr)
inline bool eq_mem(SymExpr& a, SymExpr& b) {   return a.get() == b.get();  }

// Symbolic equal   (same tree structure)
inline bool eq_sym(SymExpr& a, SymExpr& b) {   return false; }

// eq_mem is more efficient. It is done first.
inline bool equiv(SymExpr& a, SymExpr& b)  { return eq_mem(a, b) || eq_sym(a, b);  }


class UnaryOperator: public Expression
{
public:
    typedef std::function<double(double)> UnaryFunction;

    UnaryOperator(SymExpr expr):
        _expr(expr)
    {}

    virtual UnaryFunction function() = 0;

    std::ostream& print(std::ostream& out, const std::string& op) {

        if (_expr->parens()){
            out << op << '('; _expr->print(out) << ')';
        }
        else{
             out << op; _expr->print(out);
        }

        return out;
    }

    double full_eval(Context& c){
        return function()(_expr->full_eval(c));
    }

    template<typename Node>
    SymExpr partial_eval(Context& c) {

        SymExpr v = _expr->partial_eval(c);

        if (v->is_scalar())
            return ScalarDouble::make(function()(get_value(v)));

        return Node::make(v);
    }

protected:
    SymExpr _expr;
};

class BinaryOperator: public Expression
{
public:
    typedef std::function<double(double, double)> BinaryFunction;

    BinaryOperator(SymExpr lhs, SymExpr rhs):
        _lhs(lhs), _rhs(rhs)
    {}

    std::ostream& print(std::ostream& out, const std::string& op) {

        if (_lhs->parens() && _rhs->parens()){
            out << '(';
                _lhs->print(out);
            out << " " << op << " ";
                _rhs->print(out);
            out << ')';
        }
        else{
                _lhs->print(out);
            out << " " << op << " ";
                _rhs->print(out);
        }

        return out;
    }

    virtual BinaryFunction function() = 0;

    double full_eval(Context& c){
        return function()(_lhs->full_eval(c), _rhs->full_eval(c));
    }

    template<typename Node>
    SymExpr partial_eval(Context& c) {

        SymExpr lhs = _lhs->partial_eval(c);
        SymExpr rhs = _rhs->partial_eval(c);

        // simplify
        if (lhs->is_scalar() && rhs->is_scalar()){
            double r = get_value(rhs);
            double l = get_value(lhs);
            return ScalarDouble::make(function()(r, l));
        }

        // carry
        return Node::make(lhs, rhs);
    }

    virtual bool parens() {  return true;   }

protected:
    SymExpr _lhs;
    SymExpr _rhs;
};

//                             MACROS
// -----------------------------------------------------------------------------

#define DEFAULT_UNARY_MAKE(Type)  { return Expression::make<Type>(expr);     }
#define DEFAULT_BINARY_MAKE(Type) { return Expression::make<Type>(lhs, rhs); }
#define OUTLINE_MAKE ;

// Simplify if Scalar
// This can lead to approximative results
#define BINARY_SCALAR_EVAL(lhs, rhs, Node, fun) \
    {\
        if (lhs->is_scalar() && rhs->is_scalar())\
            return ScalarDouble::make(fun(get_value(lhs), get_value(rhs)));\
    \
        return Expression::make<Node>(lhs, rhs);;\
    }\

#define UNARY_SCALAR_EVAL(expr, Node, fun) \
    {\
        if (expr->is_scalar())\
            return ScalarDouble::make(fun(get_value(expr)));\
    \
        return Expression::make<Node>(expr);;\
    }\

#define EAGER_EVAL(x) x

#define DEFINE_UNARY(Name, repr_name, ExprSubTypeID, make_fun, fun, derivative)\
    class Name: public UnaryOperator{\
    public:\
        Name(SymExpr expr):\
            UnaryOperator(expr) {}\
    \
        static SymExpr make(SymExpr expr) make_fun\
    \
        UnaryFunction function() {\
            return fun;\
        }\
    \
        std::ostream& print(std::ostream& out) {\
            return UnaryOperator::print(out, repr_name);\
        }\
    \
        SymExpr partial_eval(Context &c){\
            return UnaryOperator::partial_eval<Name>(c);\
        }\
    \
        SymExpr derivate(const std::string& name) {\
            derivative\
        }\
    \
        operator ExprSubType () const  { return ExprSubTypeID; }\
    }

#define DEFINE_BINARY(Name, repr_name, ExprSubTypeID, make_fun, fun, derivative)\
    class Name: public BinaryOperator{\
    public:\
        Name(SymExpr lhs, SymExpr rhs):\
            BinaryOperator(lhs, rhs) {}\
    \
        static SymExpr make(SymExpr lhs, SymExpr rhs) make_fun\
    \
        BinaryFunction function() {\
            return fun;\
        }\
    \
        std::ostream& print(std::ostream& out) {\
            return BinaryOperator::print(out, repr_name);\
        }\
    \
        SymExpr partial_eval(Context &c){\
            return BinaryOperator::partial_eval<Name>(c);\
        }\
    \
        SymExpr derivate(const std::string& name) {\
            derivative\
        }\
    \
        operator ExprSubType () const  { return ExprSubTypeID; }\
    }

}
}
#endif
