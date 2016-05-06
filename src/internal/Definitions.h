#ifndef SYMDIF_INTERNAL_DEFINITIONS_HEADER
#define SYMDIF_INTERNAL_DEFINITIONS_HEADER
/*
 *  Description:
 *    - Define basic mathematical functions and their derivatives
 */

#include "Utils.h"
#include <cmath>

// Function name used in gen(out, OutputType)
#define SCHEME(x) x
#define CPP(x) x
#define STANDARD(x) x

namespace symdiff{
namespace internal{

//builder.CreateFDiv()
//builder.CreateFSub()
// builder.CreateFNeg()


DEFINE_BINARY(Add, "+", EST_Add, OUTLINE_MAKE,
    // Function to apply
    [](double a, double b){ return a + b; },

    // Derivative
    {
        SymExpr rhs = _rhs->derivate(name);
        SymExpr lhs = _lhs->derivate(name);
        return Add::make(rhs, lhs);
    },
    return bl.CreateAdd(_rhs->llvm_gen(bl), _lhs->llvm_gen(bl));
);

DEFINE_UNARY(Opposite, "-", EST_Neg, OUTLINE_MAKE,
    // Function to apply
    [](double v){ return - v; },

    // Derivative
    {
        return Opposite::make(_expr->derivate(name));
    },
    // Not sure if builder.CreateFNeg() is equivalent
    // return bl.CreateFSub(LLVM_MAKE_DOUBLE(0), _expr->llvm_gen(bl));
    return bl.CreateMul(LLVM_MAKE_DOUBLE(-1), _expr->llvm_gen(bl));
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
    },

    return bl.CreateMul(_rhs->llvm_gen(bl), _lhs->llvm_gen(bl));
);

DEFINE_BINARY(Pow, "^", EST_Pow, OUTLINE_MAKE,
    // Function to apply
    [](double a, double b){ return std::pow(a, b); },

    // Derivative
    {
        // (a ^ b)' = a'b' (a ^ (b - 1))
        SymExpr& power = _rhs;
        SymExpr& expr = _lhs;

        //SymExpr pp = power->derivate(name);
        SymExpr ep = expr->derivate(name);

        return Mult::make(Mult::make(power, ep), Pow::make(expr, Add::make(power, minus_one())));
    },
    //
    return LLVM_MAKE_DOUBLE(0);
);


DEFINE_UNARY(Inverse, "/", EST_Inv, OUTLINE_MAKE,
    // Function to apply
    [](double v){ return 1.0 / v; },

    // Derivative
    {
        auto up = _expr->derivate(name);
        auto down = Inverse::make(Mult::make(_expr, _expr));

        return Opposite::make(Mult::make(up, down));
    },

    return bl.CreateFDiv(LLVM_MAKE_DOUBLE(1), _expr->llvm_gen(bl));
);


DEFINE_UNARY(Exp, "exp", EST_Exp,  OUTLINE_MAKE,
    // Function to apply
    [](double v){ return std::exp(v); },

    // Derivative
    {
        auto dx = _expr->derivate(name);
        return Mult::make(dx, Exp::make(_expr));
    },
    return LLVM_MAKE_DOUBLE(0);
);

DEFINE_UNARY(Ln, "ln", EST_Ln, OUTLINE_MAKE,
    // Function to apply
    [](double v){ return std::log(v); },

    // Derivative
    {
        return Inverse::make(_expr);
    },
    return LLVM_MAKE_DOUBLE(0);
);

}
}

#endif
