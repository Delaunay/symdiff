#ifndef SYMDIF_INTERNAL_UTILS_HEADER
#define SYMDIF_INTERNAL_UTILS_HEADER
/*
 *  Descriptions:
 *    - Define Abstract Unary and Binary Nodes
 *    - Define Helper Macro that facilitate adding nodes
 */

#include "Scalar.h"

#include <vector>
#include <algorithm>

#include <functional>

namespace symdiff{
namespace internal{

// reorder is a conditional swap
void reorder(SymExpr& a, SymExpr& b);

// Memory equal     (same ptr)
inline bool eq_mem(SymExpr& a, SymExpr& b) {   return a.get() == b.get();  }

// Symbolic equal  (same tree structure)
// Symbolic equal can return false in some case where
// nodes are associative if scalar nodes are not ordered
bool eq_sym(SymExpr& a, SymExpr& b);

// eq_mem is more efficient. It is done first.
inline bool equiv(SymExpr& a, SymExpr& b)  {
    if (eq_mem(a, b))
        return true;

    return eq_sym(a, b);
}

class UnaryOperator: public Expression
{
public:
    typedef std::function<double(double)> UnaryFunction;

    UnaryOperator(SymExpr expr):
        _expr(expr)
    {}

    virtual UnaryFunction function() = 0;

    std::ostream& print(std::ostream& out, const std::string& op) {

        if (_expr->is_leaf()){
            out << op << " "; _expr->print(out);
        }
        else{
            out << op << " ("; _expr->print(out) << ")";
        }

        return out;
    }

    std::ostream& gen(std::ostream& out, const std::string& op, OutputType t) {
        switch (t){
            case Standard:
                return this->print(out, op);
            case Cpp: // CPP printing is not homogenous accros Operators
                return this->print(out, op);
            case Scheme:
                out << '(' << op << ' '; _expr->gen(out, t) << ')';
                return out;
            default:
                return this->print(out, op);
        }
    }

    double full_eval(Context& c)    {   return function()(_expr->full_eval(c)); }
    double full_eval(PtrContext& c) {   return function()(_expr->full_eval(c)); }
    int depth(int i = 0) {  return  _expr->depth(i + 1);   }

    // I think this could be done using template code
    // Must check if std::mem_fun can be used
    #define UNARY_APPLY(Node, fun) { \
            SymExpr expr = _expr->fun(c);\
            return Node::make(_expr);\
        }

    template<typename Node, typename Ctx>
    SymExpr partial_eval(Ctx& c) {
        SymExpr expr = _expr->partial_eval(c);

        if (expr->is_scalar())
            return ScalarDouble::make(function()(get_value(expr)));

        return Node::make(_expr);
    }

    template<typename Node>
    SymExpr substitute(Context& c)   {  UNARY_APPLY(Node, substitute);     }

    #undef UNARY_APPLY

    SymExpr& expr() { return _expr;    }
    bool sym_equal(SymExpr& a) { return this->get_type() == a->get_type();   }

    bool equal(SymExpr& a) {
        if (sym_equal(a)){
            UnaryOperator* o = dynamic_cast<UnaryOperator*>(a.get());
            return expr()->equal(o->expr());
        }

        if (a->is_pattern())
            return pattern_equal(a);

        return false;
    }

    bool pattern_equal(SymExpr& a) { return !a->is_binary();   }

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

        ExprSubType c = get_type();
        ExprSubType l = _lhs->get_type();
        ExprSubType r = _rhs->get_type();

        if (l < c && !_lhs->is_leaf()) out << '(';
            _lhs->print(out);
        if (l < c && !_lhs->is_leaf()) out << ')';

        // Transform +- to - and */ to /
        if ((c != EST_Mult || r != EST_Inv) && (c != EST_Add  || r != EST_Neg))
             out << " " << op;

        out << " ";

        if (r < c && !_rhs->is_leaf()) out << '(';
            _rhs->print(out);
        if (r < c && !_rhs->is_leaf()) out << ')';

        return out;
    }

    std::ostream& gen(std::ostream& out, const std::string& op, OutputType t) {
        switch (t){
            case Standard:
                return this->print(out, op);
            case Cpp:  // CPP printing is not homogenous accros Operators
                return this->print(out, op);
            case Scheme:
                out << '(' << op << ' '; _lhs->gen(out, t) << ' '; _rhs->gen(out, t) << ')';
                return out;
            default:
                return this->print(out, op);
        }
    }

    virtual BinaryFunction function() = 0;

    double full_eval(Context& c){
        return function()(_lhs->full_eval(c), _rhs->full_eval(c));
    }

    double full_eval(PtrContext& c){
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

    template<typename Node, typename Ctx>
    SymExpr partial_eval(Ctx& c) {
        SymExpr lhs = _lhs->partial_eval(c);
        SymExpr rhs = _rhs->partial_eval(c);

        if (lhs->is_scalar() && rhs->is_scalar())
            return ScalarDouble::make(function()(get_value(lhs), get_value(rhs)));

        return Node::make(lhs, rhs);

     }

    template<typename Node>
    SymExpr substitute(Context& c)   {  BINARY_APPLY(Node, substitute);     }

    #undef BINARY_APPLY

    bool parens()    {  return true;   }
    bool is_binary() {  return true;   }
    bool sym_equal(SymExpr& a) { return this->get_type() == a->get_type();   }

    SymExpr& lhs() { return _lhs;    }
    SymExpr& rhs() { return _rhs;    }
    int depth(int i = 0) {  return std::max(_lhs->depth(i + 1), _rhs->depth(i + 1));   }

    bool equal(SymExpr& a) {
        if (sym_equal(a)){
            BinaryOperator* b = dynamic_cast<BinaryOperator*>(a.get());
            return lhs()->equal(b->lhs()) && rhs()->equal(b->rhs()) ;
        }

        if (a->is_pattern()){
            return pattern_equal(a);
        }

        return false;
    }

    bool pattern_equal(SymExpr& a) { return a->is_binary();   }

protected:
    SymExpr _lhs;
    SymExpr _rhs;
};


class NnaryOperator: public Expression
{
public:
    typedef std::vector<SymExpr> Args;

    NnaryOperator(const Args& args):
        _args(args)
    {}

    bool parens()       {  return true;   }
    SymExpr& arg(int i) { return _args[i];    }

    bool sym_equal(SymExpr& a) { return this->get_type() == a->get_type();   }
    bool equal(SymExpr& a) {
        if (sym_equal(a)){
            NnaryOperator* b = dynamic_cast<NnaryOperator*>(a.get());

            if (this->size() != b->size())
                return false;

            for(int i = 0; i < size(); ++i){
                if (!(arg(i)->equal(b->arg(i))))
                    return false;
            }

            return true;
        }

        return false;
    }

    template<typename Node, typename Ctx>
    SymExpr partial_eval(Ctx& c) {
        // a function call would stuff those in a new context

        std::vector<SymExpr> pargs; pargs.reserve(size());

        std::transform(_args.begin(), _args.end(), std::back_inserter(pargs),
            [&c](SymExpr& v){
            return v->partial_eval(c);
        });

        // Hummm



        //if (lhs->is_scalar() && rhs->is_scalar())
        //    return ScalarDouble::make(function()(get_value(lhs), get_value(rhs)));

        return Node::make(pargs);

     }

    std::size_t size() const {  return _args.size();    }
    int depth(int i = 0) {  return i + 1;   }

protected:
    Args _args;
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
        std::ostream& gen(std::ostream& out, OutputType t) {\
            return UnaryOperator::gen(out, repr_name, t);\
        }\
    \
        SymExpr partial_eval(Context &c){\
            return UnaryOperator::partial_eval<Name>(c);\
        }\
    \
        SymExpr partial_eval(PtrContext &c){\
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
        std::ostream& gen(std::ostream& out, OutputType t) {\
            return BinaryOperator::gen(out, repr_name, t);\
        }\
    \
        SymExpr partial_eval(Context &c){\
            return BinaryOperator::partial_eval<Name>(c);\
        }\
    \
        SymExpr partial_eval(PtrContext &c){\
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
