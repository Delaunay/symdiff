/*
 *  Date: Dec 14 2015
 *
 *  Author: Pierre Delaunay
 *
 *  TODO: Replace naked pointers with std::unique_ptr<> for public API function
 */

#include "operators.h"

#include <exception>

namespace sym{

// Exception
//
class CastError : public std::exception
{
public:
    const char* what() const noexcept{
        return "Expression is not a Scalar\n"
               "\t 1) You are trying to extract a value from an expression with unkown\n"
               "\ttry full_eval instead\n"
               "\t 2) You are trying to extract binary operand from a non binary Expression\n";
    }
};


Bin explode(Expr x){
    ExprType t = x->type();
    BinOp* b = dynamic_cast<BinOp*>(x.get());

    if (t == tAdd || t == tMult)
        return Bin(b->left, b->right);

    throw CastError();
}
//
//  Implementation
//
Expr constant(Real v){
    return Expr(new Scalar(v));
}

Expr var(const std::string& name){
    return Expr(new Variable(name));
}

Variable& var(Expr x){
    return (Variable&) *(x.get());
}

Expr exp(Expr l){
    if (l->is_nul())
        return one();

    return Expr(new Exp(l));
}

Expr scalar(Real v){
    return Expr(new Scalar(v));
}

Expr pow(Expr e, Expr p){
    return Expr(new Pow(e, p));
}
Expr pow(Expr e, Real p){
    return Expr(new Pow(e, Expr(new Scalar(p))));
}

Real val(Expr x){
    if (x->is_scalar())
        return dynamic_cast<Scalar*>(x.get())->value();

    throw CastError();
}

Real val_nocheck(Expr x){
    return dynamic_cast<Scalar*>(x.get())->value();
}

//Expr pi(){
//    static MathConst pi_val("pi", 3.141592654);
//    return (Expr) &pi_val;
//}

// so I don't think it is interesting to handle de x/0 case
// since it is symbolic computing as long as we don't eval it who cares
// + we might want to compute some limit or something
Expr inv(Expr l){
    if (l->is_one())
        return one();

    return Expr(new Inverse(l));
}

Expr ln(Expr l){
    if (l->is_one())
        return zero();

    return Expr(new Ln(l));
}

Expr mult(Expr l, Expr r){
    if (l->is_nul() || r->is_nul())
        return zero();

    if (l->is_one())
        return r;

    if (r->is_one())
        return l;

    // if scalar evaluate
    if (r->is_scalar() && l->is_scalar())
        return Expr(new Scalar(val_nocheck(r) * val_nocheck(l)));

    return Expr(new Mult(l, r));
}

Expr add(Expr l, Expr r){
    if (l->is_nul() && r->is_nul())
        return zero();

    if (l->is_nul())
        return r;

    if (r->is_nul())
        return l;

    // simplify
    if (r->is_scalar() && l->is_scalar())
        return Expr(new Scalar(val_nocheck(r) + val_nocheck(l)));

    // identical ?
    if (l == r)
        return mult(Expr(new Scalar(2)), l);

    return Expr(new Add(l, r));
}

}
