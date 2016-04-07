#ifndef SYMDIF_DEFINITIONS_HEADER
#define SYMDIF_DEFINITIONS_HEADER
/*
 *  Description:
 *    - Define basic mathematical functions and their derivatives
 */

#include "Utils.h"
#include <cmath>

namespace symdiff{
namespace internal{

DEFINE_BINARY(Add, "+", EST_Add, OUTLINE_MAKE,
    // Function to apply
    [](double a, double b){ return a + b; },

    // Derivative
    {
        SymExpr rhs = _rhs->derivate(name);
        SymExpr lhs = _lhs->derivate(name);
        return Add::make(rhs, lhs);
    }
);

DEFINE_UNARY(Opposite, "-", EST_Neg, DEFAULT_UNARY_MAKE(Opposite),

    // function to apply
    [](double v){ return - v; },

    // Derivative
    {
        return Opposite::make(_expr->derivate(name));
    }
);

DEFINE_BINARY(Mult, "*", EST_Mult, OUTLINE_MAKE,
    // Function to apply
    [](double a, double b){ return a * b; },

    // Derivative
    {
        SymExpr rhs = _rhs->derivate(name);
        SymExpr lhs = _lhs->derivate(name);

        auto first = Mult::make(rhs, _lhs);
        auto sec = Mult::make(_rhs, lhs);

        return Add::make(first, sec);
    }
);

DEFINE_UNARY(Inverse, "1 / ", EST_Inv, DEFAULT_UNARY_MAKE(Inverse),
    // function to apply
    [](double v){ return 1.0 / v; },

    // Derivative
    {
        auto up = _expr->derivate(name);
        auto down = Inverse::make(Mult::make(_expr, _expr));

        return Opposite::make(Mult::make(up, down));
    }
);

DEFINE_UNARY(Exp, "exp", EST_Exp,  DEFAULT_UNARY_MAKE(Exp),
    // function to apply
    [](double v){ return std::exp(v); },

    // Derivative
    {
        auto dx = _expr->derivate(name);
        return Mult::make(dx, Exp::make(_expr));
    }
);

DEFINE_UNARY(Ln, "ln", EST_Ln, DEFAULT_UNARY_MAKE(Ln),
    // function to apply
    [](double v){ return std::log(v); },

    // Derivative
    {
        return Inverse::make(_expr);
    }
);

}
}

#endif
