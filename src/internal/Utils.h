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

    // I think this could be done using template code
    // Must check if std::mem_fun can be used
    #define UNARY_APPLY(Node, fun) { \
            SymExpr expr = _expr->fun(c);\
            return Node::make(_expr);\
        }

    template<typename Node>
    SymExpr partial_eval(Context& c) {
        SymExpr expr = _expr->partial_eval(c);

        if (expr->is_scalar())
            return ScalarDouble::make(function()(get_value(expr)));

        return Node::make(_expr);
    }

    template<typename Node>
    SymExpr substitute(Context& c)   {  UNARY_APPLY(Node, substitute);     }

    #undef UNARY_APPLY

    SymExpr& expr() { return _expr;    }

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

        // as far as I can tell this works
        // if parens are not present this most likely the place to check
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

    // I think this could be done using template code
    // Must check if std::mem_fun can be used
    #define BINARY_APPLY(Node, fun) { \
            SymExpr lhs = _lhs->fun(c);\
            SymExpr rhs = _rhs->fun(c);\
        \
            return Node::make(lhs, rhs);\
        }

    template<typename Node>
    SymExpr partial_eval(Context& c) {
        SymExpr lhs = _lhs->partial_eval(c);
        SymExpr rhs = _rhs->partial_eval(c);

        if (lhs->is_scalar() && rhs->is_scalar())
            return ScalarDouble::make(function()(get_value(lhs), get_value(rhs)));

        return Node::make(lhs, rhs);

     }

    template<typename Node>
    SymExpr substitute(Context& c)   {  BINARY_APPLY(Node, substitute);     }

    #undef BINARY_APPLY

    virtual bool parens() {  return true;   }

    SymExpr& lhs() { return _lhs;    }
    SymExpr& rhs() { return _rhs;    }

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
        SymExpr substitute(Context& c) {\
            return UnaryOperator::substitute<Name>(c);\
        }\
    \
        ExprSubType get_type() const  { return ExprSubTypeID; }\
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
        SymExpr substitute(Context& c) {\
            return BinaryOperator::substitute<Name>(c);\
        }\
    \
        ExprSubType get_type() const  { return ExprSubTypeID; }\
    }

}
}
#endif
