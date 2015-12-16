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

Expression* constant(Real v);
Expression* var(const std::string& name);
Variable& var(Expression* x);


// it is better to use those function instead of the AST
// since those function will simplify the expression when possible
Expression* add(Expr* l, Expr* r);
Expression* mult(Expr* l, Expr* r);

Expression* exp(Expr* l);
Expression* inv(Expr* v);
Expression* ln(Expr* l);

Expression* pi();

// retrieve a value from a Scalar expression
Real val(Expression* x);
Real val_nocheck(Expression* x);


//
//  Basic Operation
//
class Add: public BinOp
{
    public:
    Add(Expr* l, Expr* r):
        BinOp(l, r)
    {}

    ExprType type() override {  return tAdd;}

    // (U + V)' = U' + V'
    Expr* derivate(Variable& x) override {
        return add(right->derivate(x), left->derivate(x));
    }

    Expr* eval(Variables& v) override {
        Expr* el = left->eval(v);
        Expr* er = right->eval(v);
        Real vl = 0;
        Real vr = 0;

        if (el->is_scalar()){
           vl = dynamic_cast<Scalar*>(el)->value();

           if (er->is_scalar()){
               vr = dynamic_cast<Scalar*>(er)->value();
               return new Scalar(vr + vl);
           }

           return new Add(new Scalar(vl), er);
        }

        if (er->is_scalar()){
            vr = dynamic_cast<Scalar*>(er)->value();
            return new Add(el, new Scalar(vr));
        }

        return new Add(el, er);
    }

    Real full_eval(Variables& x) override{
        return left->full_eval(x) + right->full_eval(x);
    }

    std::ostream& print(std::ostream& out) override {
        left->print(out); out << " + "; right->print(out);
        return out;
    }

    Expr* simplify() override{

        if (left->is_nul())
            return right;

        if (right->is_nul())
            return left;

        if (right->is_scalar() && left->is_scalar())
            return new Scalar(val_nocheck(left) + val_nocheck(right));

        return this;
    }
};


class Mult: public BinOp
{
    public:
    typedef std::multiplies<Real> f;

    Mult(Expr* l, Expr* r):
        BinOp(l, r)
    {}

    ExprType type() override {  return tMult;}

    // (U . V)' = U'.V + V'.U
    Expr* derivate(Variable& x) override {
        return add(mult(right->derivate(x), left),
                   mult(left->derivate(x), right));
    }

    Expr* eval(Variables& v) override {
        Expr* el = left->eval(v);
        Expr* er = right->eval(v);
        Real vl = 0;
        Real vr = 0;

        if (el->is_scalar()){
           vl = dynamic_cast<Scalar*>(el)->value();

           if (er->is_scalar()){
               vr = dynamic_cast<Scalar*>(er)->value();
               return new Scalar(vr * vl);
           }

           return new Mult(new Scalar(vl), er);
        }

        if (er->is_scalar()){
            vr = dynamic_cast<Scalar*>(er)->value();
            return new Mult(el, new Scalar(vr));
        }

        return new Mult(el, er);
    }

    Real full_eval(Variables& x) override{
        return left->full_eval(x) * right->full_eval(x);
    }

    std::ostream& print(std::ostream& out) override {

        // pretty print
        if (left->is_scalar()){
            left->print(out); out << "("; right->print(out); out << ")";
            return out;
        }

        if (right->is_scalar()){
            right->print(out); out << "("; left->print(out); out << ")";
            return out;
        }

        // generic print
        out << "("; left->print(out); out << ") * ("; right->print(out); out << ")";
        return out;
    }

    Expr* simplify() override{

        if (left->is_nul() || right->is_nul())
            return zero();

        if (left->is_one())
            return right;

        if (right->is_one())
            return left;

        if (right->is_scalar() && left->is_scalar())
            return new Scalar(val_nocheck(left) * val_nocheck(right));

        return this;
    }
};

//
//  e^(x)
//
class Exp: public UnaryOp
{
public:
    Exp(Expr* v):
        UnaryOp(v)
    {}

    ExprType type() override {  return tExp;}

    // (exp(x))' = x' exp(x)
    Expr* derivate(Variable& x) override {
        return mult(expr->derivate(x), exp(expr));
    }

    Expr* eval(Variables& x) override {
        //return _eval<Exp, std::exp<Real> >(v);
        Expr* e = expr->eval(x);

        if (e->is_scalar()){
            Real v = dynamic_cast<Scalar*>(e)->value();

            return new Scalar(std::exp(v));
        }

        return exp(e);
    }

    Real full_eval(Variables& x) override{
        return std::exp(expr->full_eval(x));
    }

    std::ostream& print(std::ostream& out) override {
        out << "exp("; expr->print(out); out << ")";
        return out;
    }

    Expr* simplify() override{

        if (expr->is_nul())
            return one();

        if (expr->type() == tLn)
            return expr;

        /* We might not want to do evaluate a non exact value
        if (expr->is_scalar())
            return new Scalar(std::exp(val_nocheck(expr))); //*/

        return this;
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

    Inverse(Expr* v):
        UnaryOp(v)
    {}

    ExprType type() override {  return tInv;}

    // (1/u)' = - u' / u^2
    Expr* derivate(Variable& x) override {
        return mult(mult(minus_one(), expr->derivate(x)), inv(mult(expr, expr)));
    }

    //
    Expr* eval(Variables& x) override {

        Expr* e = expr->eval(x);

        if (e->is_scalar()){
            Real v = dynamic_cast<Scalar*>(e)->value();
            return new Scalar(1. / v);
        }

        return new Inverse(e);
    }

    Real full_eval(Variables& x) override{
        return 1. / expr->full_eval(x);
    }

    std::ostream& print(std::ostream& out) override {
        out << "1 / ("; expr->print(out); out << ")";
        return out;
    }

    Expr* simplify() override{

        if (expr->is_one())
            return one();

        /* We might not want to do evaluate a non exact value
        if (expr->is_scalar())
            return new Scalar(1 / (val_nocheck(expr))); //*/

        return this;
    }
};

class Ln: public UnaryOp
{
public:
    //typedef std::log f;

    Ln(Expr* v):
        UnaryOp(v)
    {}

    ExprType type() override {  return tLn;}

    // (ln(u))' = 1/u
    Expr* derivate(Variable& x) override {
        Expr* e = expr->derivate(x);

        // if e is null that mean, expr is not function of x
        if (e->is_nul())
            return zero();

        return inv(expr);
    }

    //
    Expr* eval(Variables& x) override {
        //return _eval<Ln, std::log>(v);

        Expr* e = expr->eval(x);

        if (e->is_scalar()){
            Real v = dynamic_cast<Scalar*>(e)->value();
            return new Scalar(std::log(v));
        }

        return new Ln(e);
    }

    Real full_eval(Variables& x) override{
        return std::log(expr->full_eval(x));
    }

    std::ostream& print(std::ostream& out) override {
        out << "ln("; expr->print(out); out << ")";
        return out;
    }

    Expr* simplify() override{

        if (expr->is_one())
            return zero();

        if (expr->type() == tExp)
            return expr;

        /* We might not want to do evaluate a non exact value
        if (expr->is_scalar())
            return new Scalar(std::log(val_nocheck(expr))); //*/

        return this;
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

    std::ostream& print(std::ostream& out) override{
        out << _name;
        return out;
    }

    const std::string& name() const {   return _name;   }

private:
    std::string _name;
};

}

#endif // OPERATORS_H

