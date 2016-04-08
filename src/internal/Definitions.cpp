#include "Definitions.h"

namespace symdiff{
namespace internal{

SymExpr Add::make(SymExpr lhs, SymExpr rhs){
    reorder(lhs, rhs);

    // Simplify here
    if (lhs->is_nul())  return rhs;
    if (rhs->is_nul())  return lhs;

    if (equiv(lhs, rhs))
        return Mult::make(two(), lhs);

    // Base Case
    if (lhs->is_scalar() && rhs->is_scalar())
        return ScalarDouble::make(get_value(lhs) + get_value(rhs));

    return Expression::make<Add>(lhs, rhs);
}


#define UNARY_SCALAR_EVAL(expr, Node, fun) \
    {\
        if (expr->is_scalar())\
            return ScalarDouble::make(fun(get_value(expr)));\
    \
        return Expression::make<Node>(expr);;\
    }\

SymExpr Mult::make(SymExpr lhs, SymExpr rhs){
    reorder(lhs, rhs);

    // Simplify here
    if (lhs->is_nul() || rhs->is_nul())
        return zero();

    if (lhs->is_one())  return rhs;
    if (rhs->is_one())  return lhs;

    //if (equiv(lhs, rhs))
    //    return Pow::make(two(), rhs);

    // Base Case
    if (lhs->is_scalar() && rhs->is_scalar())
        return ScalarDouble::make(get_value(lhs) * get_value(rhs));

    return Expression::make<Mult>(lhs, rhs);
}

SymExpr Opposite::make(SymExpr expr){
    if (expr->is_scalar()){
        return ScalarDouble::make(- get_value(expr));
    }

    return Expression::make<Opposite>(expr);
}

SymExpr Inverse::make(SymExpr expr){
    if (expr->is_scalar()){
        return ScalarDouble::make(1.0 / get_value(expr));
    }

    return Expression::make<Inverse>(expr);
}

SymExpr Exp::make(SymExpr expr){
    if (expr->is_scalar()){
        return ScalarDouble::make(std::exp(get_value(expr)));
    }

    return Expression::make<Exp>(expr);
}

SymExpr Ln::make(SymExpr expr){
    if (expr->is_scalar()){
        return ScalarDouble::make(std::log(get_value(expr)));
    }

    return Expression::make<Ln>(expr);
}



}
}
