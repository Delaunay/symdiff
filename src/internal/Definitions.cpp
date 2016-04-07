#include "Definitions.h"

namespace symdiff{
namespace internal{

SymExpr Add::make(SymExpr lhs, SymExpr rhs){
    reorder(lhs, rhs);

    // Simplify here
    if (lhs->is_nul())  return rhs;
    if (rhs->is_nul())  return lhs;

    // Scalar
    if (lhs->is_scalar() && rhs->is_scalar())
        return ScalarDouble::make(get_value(lhs) + get_value(rhs));

    if (equiv(lhs, rhs))
        return Mult::make(two(), lhs);

    // Base Case
    return Expression::make<Add>(lhs, rhs);
}

SymExpr Mult::make(SymExpr lhs, SymExpr rhs){
    reorder(lhs, rhs);

    // Simplify here
    if (lhs->is_nul() || rhs->is_nul())
        return zero();

    if (lhs->is_one())  return rhs;
    if (rhs->is_one())  return lhs;

    // Scalar
    if (lhs->is_scalar() && rhs->is_scalar())
        return ScalarDouble::make(get_value(lhs) * get_value(rhs));

    //if (equiv(lhs, rhs))
    //    return Pow::make(two(), rhs);

    // Base Case
    return Expression::make<Mult>(lhs, rhs);
}

}
}
