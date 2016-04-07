#include "Expression.h"


namespace symdiff{
namespace internal {

void reorder(SymExpr& a, SymExpr& b){

    ExprSubType ta = *a;
    ExprSubType tb = *b;

    // nothing to do
    if (ta < tb)
        return;

    // (1 * 0) => (0 * 1)
    if (ta == EST_Scalar && ta == tb){
        if (get_value(a) < get_value(b))
            return;
    }

    std::swap(a, b);
}

SymExpr Add::make(SymExpr lhs, SymExpr rhs){
    reorder(lhs, rhs);

    // Simplify here
    if (lhs->is_nul())  return rhs;
    if (rhs->is_nul())  return lhs;

    // Scalar
    if (lhs->is_scalar() && rhs->is_scalar())
        return ScalarDouble::make(get_value(lhs) + get_value(rhs));

    // same memory segement
    if (*lhs == *rhs)
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

    // same memory segement
    //if (*lhs == *rhs)
    //    return Pow::make(lhs, two());

    // Base Case
    return Expression::make<Mult>(lhs, rhs);
}

//                            Constant
// -----------------------------------------------------------------------------

SymExpr minus_one(){
    static SymExpr o = Expression::make<ScalarDouble>(-1);
    return o;
}

SymExpr zero(){
    static SymExpr z = Expression::make<ScalarDouble>(0);
    return z;
}

SymExpr one(){
    static SymExpr o = Expression::make<ScalarDouble>(1);
    return o;
}


SymExpr two(){
    static SymExpr o = Expression::make<ScalarDouble>(2);
    return o;
}

double get_value(SymExpr v){
    if (v->is_scalar()){
        ScalarDouble* e = dynamic_cast<ScalarDouble*>(v.get());
        return e->value();
    }
    throw CastError("Expression does not hold a value");
}

const std::string&  get_name(SymExpr v){

    // I doubt switching is more efficient than a 'if(v->is_placeholder())'
    // if it is only over one type

    // but if we switch over n type we are going to make
    // less call to virtual functions (1) instead of (n)
    // (supposing we have implemented n 'is_type()')
    switch(*v){
        case EST_Placeholder:{
            Placeholder* p = dynamic_cast<Placeholder*>(v.get());
            return p->name();
        }

        default:
            throw CastError("Expression does not hold a name");
    }
}


}
}
