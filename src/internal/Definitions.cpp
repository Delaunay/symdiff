#include "Definitions.h"

namespace symdiff{
namespace internal{

SymExpr Add::make(SymExpr lhs, SymExpr rhs){
    reorder(lhs, rhs);

    // Simplify here
    if (lhs->is_nul())  return rhs;
    if (rhs->is_nul())  return lhs;

    // NB: it might be better to have a special Addflat
    // which flatten chained addition and collapse them later
    // A Flatten node can be useful for mult too!
    // we could simplify : (x + 2) + 3 easily

    //      Add Collapse
    // -----------------------
    // /!\ Scalar Node Order

    // (a + a + a + a)
    // (a + (a + (a + a)))
    // (a + (a + (2 * a)))

    if (equiv(lhs, rhs))
        return Mult::make(two(), lhs);

    // This is where ordered term become useful.
    // I don't have to check for all possible cases
    if (rhs->get_type() == EST_Mult){
        Mult* m = dynamic_cast<Mult*>(rhs.get());

        if (m->lhs()->is_scalar() && equiv(m->rhs(), lhs)){
            return Mult::make(ScalarDouble::make(get_value(m->lhs()) + 1), lhs);
        }
    }

    // (a - a)
    // (a + Neg(a))
    if (rhs->get_type() == EST_Neg){
        Opposite* o = dynamic_cast<Opposite*>(rhs.get());

        if (equiv(lhs, o->expr()))
            return zero();
    }

    return Expression::make<Add>(lhs, rhs);
}

SymExpr Mult::make(SymExpr lhs, SymExpr rhs){
    reorder(lhs, rhs);

    // Simplify here
    if (lhs->is_nul() || rhs->is_nul())
        return zero();

    if (lhs->is_one())  return rhs;
    if (rhs->is_one())  return lhs;

    //      Mult Collapse
    // -----------------------
    // /!\ Scalar Node Order

    // (a * a * a * a)
    // (a * (a * (a * a)))
    // (a * (a * (a ^ 2)))

    // Base case
    if (equiv(lhs, rhs))
        return Pow::make(rhs, two());

    if (rhs->get_type() == EST_Pow){
        Pow* m = dynamic_cast<Pow*>(rhs.get());

        if (m->rhs()->is_scalar() && equiv(m->lhs(), lhs)){
            return Pow::make(lhs, ScalarDouble::make(get_value(m->rhs()) + 1));
        }
    }

    // (a / a)
    // (a * inv(a))
    if (rhs->get_type() == EST_Inv){
        Inverse* o = dynamic_cast<Inverse*>(rhs.get());

        if (equiv(lhs, o->expr()))
            return one();
    }

    return Expression::make<Mult>(lhs, rhs);
}

SymExpr Pow::make(SymExpr expr, SymExpr power){
    // Simplify here
    if (expr->is_nul())
        return zero();

    if (power->is_nul())
        return one();

    if (power->is_one())  return expr;
    if (expr->is_one())  return one();

    // (x ^ y) ^ z = (x ^ (y * z))
    if (expr->get_type() == EST_Pow){
        Pow* m = dynamic_cast<Pow*>(expr.get());
        return Pow::make(m->lhs(), Mult::make(m->rhs(), power));
    }

    return Expression::make<Pow>(expr, power);
}


SymExpr Opposite::make(SymExpr expr){
    // Simplify Here
    if (expr->is_nul())
        return zero();

    if (expr->get_type() == EST_Neg){
        Opposite* o = dynamic_cast<Opposite*>(expr.get());
        return o->expr();
    }

    return Expression::make<Opposite>(expr);
}

SymExpr Inverse::make(SymExpr expr){
    // Simplify Here
    if (expr->is_one())
        return one();

    if (expr->get_type() == EST_Inv){
        Inverse* o = dynamic_cast<Inverse*>(expr.get());
        return o->expr();
    }

    return Expression::make<Inverse>(expr);
}

SymExpr Exp::make(SymExpr expr){
    // Simplify Here
    if (expr->is_one())
        return e();

    if (expr->is_nul())
        return one();

    if (expr->get_type() == EST_Ln){
        UnaryOperator* o = dynamic_cast<UnaryOperator*>(expr.get());
        return o->expr();
    }

    return Expression::make<Exp>(expr);
}

SymExpr Ln::make(SymExpr expr){
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

}
}
