/*
 *  Date: Dec 14 2015
 *
 *  Author: Pierre Delaunay
 *
 *  File: Define Basic Mathematic Operation
 *      - add, mult, exp, inv, ln
 *
 *  TODO: Replace naked pointers with std::unique_ptr<> for public API function
 */
#ifndef OPERATORS_H
#define OPERATORS_H

#include <cmath>
#include <functional>

#include "expression.h"

namespace sym{

Expr constant(Real v);
Expr var(const std::string& name);
Variable& var(Expr x);


// it is better to use those function instead of the AST
// since those function will simplify the expression when possible
Expr add(Expr l, Expr r);
Expr mult(Expr l, Expr r);

Expr exp(Expr l);
Expr inv(Expr v);
Expr ln(Expr l);

Expr pow(Expr e, Expr p);
Expr pow(Expr e, Real p);

Expr scalar(Real);

//Expr pi();

// retrieve a value from a Scalar expression
Real val(Expr x);
Real val_nocheck(Expr x);

struct Bin
{
    Bin(Expr l, Expr r):
        left(l), right(r)
    {}

    Expr left;
    Expr right;
};


Bin explode(Expr x);

//
//  Basic Operation
//
class Add: public BinOp
{
    public:
    Add(Expr l, Expr r):
        BinOp(l, r)
    {}

    TYPE override {  return tAdd;}

    // (U + V)' = U' + V'
    DERIVATE override {
        Expr l = TB( left->derivate(tb, x));
        Expr r = TB(right->derivate(tb, x));
        return TB(add(r, l));
    }

    EVAL override {
        Expr el = left->eval(tb, x);
        Expr er = right->eval(tb, x);
        Real vl = 0;
        Real vr = 0;

        if (el->is_scalar()){
            vl = val_nocheck(el);
            delete_expr(el);

            if (er->is_scalar()){
               vr = val_nocheck(er);
               delete_expr(er);
               return TB(scalar(vr + vl));
            }

            return TB(add(TB(scalar(vl)), er));
        }

        if (er->is_scalar()){
            vr = val_nocheck(er);
            delete_expr(er);
            return TB(add(el, TB(scalar(vr))));
        }

        return TB(add(el, er));
    }

    FULL_EVAL override{
        return left->full_eval(x) + right->full_eval(x);
    }

    PRINT override {
        left->print(out); out << " + "; right->print(out);
        return out;
    }

    SIMPLIFY override{

        if (left->is_nul())
            return right;

        if (right->is_nul())
            return left;

        if (right->is_scalar() && left->is_scalar())
            return scalar(val_nocheck(left) + val_nocheck(right));

        return Expr(this);
    }

    COPY override{
        Expr r = TB(right->copy(tb));
        Expr l = TB(left->copy(tb));

        return TB(add(l, r));
    }
};


class Mult: public BinOp
{
    public:
    typedef std::multiplies<Real> f;

    Mult(Expr l, Expr r):
        BinOp(l, r)
    {}

    TYPE override {  return tMult;}

    // (U . V)' = U'.V + V'.U
    DERIVATE override {

        Expr dr = right->derivate(tb, x);
        TB(right->copy(tb));
        TB(dr);

        Expr dl = left->derivate(tb, x);
        TB(left->copy(tb));
        TB(dr);

        Expr r = mult(dr, left);
        TB(r);

        Expr l = mult(dl, right);
        TB(l);

        return TB(add(l, r));
    }

    EVAL override {
        Expr el = left->eval(tb, x);
        Expr er = right->eval(tb, x);
        Real vl = 0;
        Real vr = 0;

        if (el->is_scalar()){
            vl = val_nocheck(el);
            delete_expr(el);

            if (er->is_scalar()){
                vr = val_nocheck(er);
                delete_expr(er);
                return TB(scalar(vr * vl));
            }

            return TB(mult(TB(scalar(vl)), er));
        }

        if (er->is_scalar()){
            vr = val_nocheck(er);
            delete_expr(er);
            return TB(mult(el, TB(scalar(vr))));
        }

        return TB(mult(el, er));
    }

    FULL_EVAL override{
        return left->full_eval(x) * right->full_eval(x);
    }

    PRINT override {

        // pretty print
        if (left->is_scalar()){
            left->print(out);

            if (right->type() == tVariable){
                right->print(out);
                return out;
            }

            out << "("; right->print(out); out << ")";
            return out;
        }

        if (right->is_scalar()){
            right->print(out);

            if (left->type() == tVariable){
                left->print(out);
                return out;
            }

            out << "("; left->print(out); out << ")";
            return out;
        }


        if (left->type() == tVariable){
            left->print(out);
        }
        else{
            out << "("; left->print(out) << ")";
        }

        out << " * ";

        if (right->type() == tVariable){
            right->print(out);
        }
        else{
            out << "("; right->print(out); out << ")";
        }

        return out;
    }

    SIMPLIFY override{

        if (left->is_nul() || right->is_nul())
            return zero();

        if (left->is_one())
            return right;

        if (right->is_one())
            return left;

        if (right->is_scalar() && left->is_scalar())
            return scalar(val_nocheck(left) * val_nocheck(right));

        return Expr(this);
    }

    COPY override{
        Expr r = TB(right->copy(tb));
        Expr l = TB(left->copy(tb));

        return TB(mult(l, r));
    }
};

//
//  e^(x)
//
class Exp: public UnaryOp
{
public:
    Exp(Expr v):
        UnaryOp(v)
    {}

    TYPE override {  return tExp;}

    // (exp(x))' = x' exp(x)
    DERIVATE override {
        return mult(expr->derivate(tb, x), exp(expr));
    }

    EVAL override {
        //return _eval<Exp, std::exp<Real> >(v);
        Expr e = expr->eval(tb, x);

        if (e->is_nul()){
            delete_expr(e);
            return one();
        }

        if (e->is_scalar()){
            Real v = val_nocheck(e);
            delete_expr(e);
            return scalar(std::exp(v));
        }

        return exp(e);
    }

    FULL_EVAL override{
        return std::exp(expr->full_eval(x));
    }

    PRINT override {
        out << "exp("; expr->print(out); out << ")";
        return out;
    }

    SIMPLIFY override{

        if (expr->is_nul())
            return one();

        if (expr->type() == tLn)
            return expr;

        /* We might not want to do evaluate a non exact value
        if (expr->is_scalar())
            return new Scalar(std::exp(val_nocheck(expr))); //*/

        return Expr(this);
    }

    COPY override{
        Expr r = TB(expr->copy(tb));
        return TB(exp(r));
    }
};


template<typename _Tp>
struct inverse : public std::binary_function<_Tp, _Tp, _Tp>
{
    _Tp operator()(const _Tp& __y) const{
        return _Tp(1) / __y;
    }
};

//
//  1/x
//
class Inverse: public UnaryOp
{
public:
    // typedef inverse<Real> f;

    Inverse(Expr v):
        UnaryOp(v)
    {}

    TYPE override {  return tInv;}

    // (1/u)' = - u' / u^2
    DERIVATE override {
        return mult(mult(minus_one(), expr->derivate(tb, x)), inv(mult(expr, expr)));
    }

    //
    EVAL override {

        Expr e = expr->eval(tb, x);

        if (e->is_scalar()){
            Real v = val_nocheck(e);
            delete_expr(e);
            return scalar(1. / v);
        }

        return inv(e);
    }

    FULL_EVAL override{
        return 1. / expr->full_eval(x);
    }

    PRINT override {
        out << "1 / ("; expr->print(out); out << ")";
        return out;
    }

    SIMPLIFY override{

        if (expr->is_one())
            return one();

        /* We might not want to do evaluate a non exact value
        if (expr->is_scalar())
            return new Scalar(1 / (val_nocheck(expr))); //*/

        return Expr(this);
    }

    COPY override{
        Expr r = TB(expr->copy(tb));
        return TB(inv(r));
    }
};

class Ln: public UnaryOp
{
public:
    //typedef std::log f;

    Ln(Expr v):
        UnaryOp(v)
    {}

    TYPE override {  return tLn;}

    // (ln(u))' = 1/u
    DERIVATE override {
        Expr e = expr->derivate(tb, x);

        // if e is null that mean, expr is not function of x
        if (e->is_nul()){
            delete_expr(e);
            return zero();
        }

        if (e->is_one()){
            delete_expr(e);
            return inv(expr);
        }

        return mult(e, inv(expr));
    }

    //
    EVAL override {
        //return _eval<Ln, std::log>(v);

        Expr e = expr->eval(tb, x);

        if (e->is_scalar()){
            Real v = val_nocheck(e);
            delete_expr(e);
            return scalar(std::log(v));
        }

        return ln(e);
    }

    FULL_EVAL override{
        return std::log(expr->full_eval(x));
    }

    PRINT override {
        out << "ln("; expr->print(out); out << ")";
        return out;
    }

    SIMPLIFY override{

        if (expr->is_one())
            return zero();

        if (expr->type() == tExp)
            return expr;

        /* We might not want to do evaluate a non exact value
        if (expr->is_scalar())
            return new Scalar(std::log(val_nocheck(expr))); //*/

        return Expr(this);
    }

    COPY override{
        Expr r = TB(expr->copy(tb));
        return TB(ln(r));
    }
};

//
//  Mathematical Constant
//          a Scalar with a name
//
class MathConst: public Scalar
{
public:

    MathConst(std::string name, Real value):
        Scalar(value),_name(name)
    {}

    const std::string& name() const {   return _name;   }

    // math const are kept
    COPY override{
        return Expr(this);
    }

    PRINT override{
        out << _name;
        return out;
    }

private:
    std::string _name;
};

class Pow: public BinOp
{
public:

    Pow(Expr exp, Expr po):
        BinOp(exp, po)
    {}

    TYPE override {  return tPow;}

    DERIVATE override {
        // check if expr() is function of x
        Expr d = expr()->derivate(tb, x);

        if (d->is_nul()){
            delete_expr(d);
            return zero();
        }

        if (d->is_one()){
            delete_expr(d);
            return mult(power(), pow(expr(), add(power(), minus_one())));
        }

        // if it is we can derivate
        return mult(d, mult(power(), pow(expr(), add(power(), minus_one()))));
    }

    PRINT override {
        out << "("; expr()->print(out); out << ") ^ (";
        power()->print(out); out << ")";
        return out;
    }

    COPY override{
        return TB(pow(expr()->copy(tb), power()->copy(tb)));
    }

    FULL_EVAL override{
        return std::pow(expr()->full_eval(x), power()->full_eval(x));
    }

    EVAL override {

        Expr p = power()->eval(tb, x);
        Expr e = expr()->eval(tb, x);

        if (p->is_scalar()){
            if (e->is_scalar()){
                Real ve = val_nocheck(e);
                Real vp = val_nocheck(p);

                delete_expr(e);
                delete_expr(p);

                return TB(scalar(std::pow(ve, vp)));
            }
            Real vp = val_nocheck(p);
            delete_expr(p);
            return TB(pow(e, TB(scalar(vp))));
        }

        if (e->is_scalar()){
            Real ve = val_nocheck(e);
            delete_expr(e);
            return TB(pow(scalar(ve), p));
        }

        return pow(e, p);
    }

    SIMPLIFY override{

        if (power()->is_one())
            return expr();

        if (power()->is_nul())
            return one();

        return Expr(this);
    }

    Expr power()  {  return right;   }
    Expr expr() {  return left;    }
};

}

#endif // OPERATORS_H

