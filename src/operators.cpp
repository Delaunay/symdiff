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
               "You are trying to extract a value from an expression with unkown\n"
               "try full_eval instead\n";
    }
};

//
//  Implementation
//
Expression* constant(Real v){
    return new Scalar(v);
}

Expression* var(const std::string& name){
    return new Variable(name);
}

Variable& var(Expression* x){
    return (Variable&) *x;
}

Expression* exp(Expr* l){
    if (l->is_nul())
        return one();

    return new Exp(l);
}

Real val(Expression* x){
    if (x->is_scalar())
        return dynamic_cast<Scalar*>(x)->value();

    throw CastError();
}

Real val_nocheck(Expression* x){
    return dynamic_cast<Scalar*>(x)->value();
}

Expression* pi(){
    static MathConst pi_val("pi", 3.141592654);
    return (Expr*) &pi_val;
}

// so I don't think it is interesting to handle de x/0 case
// since it is symbolic computing as long as we don't eval it who cares
// + we might want to compute some limit or something
Expression* inv(Expr* l){
    if (l->is_one())
        return one();

    return new Inverse(l);
}

Expression* ln(Expr* l){
    if (l->is_one())
        return zero();

    return new Ln(l);
}

Expression* mult(Expr* l, Expr* r){
    if (l->is_nul() || r->is_nul())
        return zero();

    if (l->is_one())
        return r;

    if (r->is_one())
        return l;

    // if scalar evaluate
    if (r->is_scalar() && l->is_scalar())
        return new Scalar(val_nocheck(r) * val_nocheck(l));

    return new Mult(l, r);
}

Expression* add(Expr* l, Expr* r){
    if (l->is_nul() && r->is_nul())
        return zero();

    if (l->is_nul())
        return r;

    if (r->is_nul())
        return l;

    // simplify
    if (r->is_scalar() && l->is_scalar())
        return new Scalar(val_nocheck(r) + val_nocheck(l));

    // identical ?
    if (l == r)
        return mult(new Scalar(2), l);

    return new Add(l, r);
}

}
