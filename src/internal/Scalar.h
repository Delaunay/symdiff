#ifndef SYMDIF_SCALAR_HEADER
#define SYMDIF_SCALAR_HEADER
/*
 *  Description:
 *    - Define a Constant Value
 */

#include "AbstractExpression.h"

namespace symdiff {
namespace internal{

// retrieve a value from a Scalar
double get_value(SymExpr v);

// Pre allocate common variables
SymExpr minus_one();
SymExpr zero();
SymExpr one();
SymExpr two();
SymExpr pi();
SymExpr e();

template<typename T>
class Scalar: public Expression
{
public:
    Scalar(T v):
        _value(v)
    {}

    static SymExpr make(T v){
        if (v == 0)
            return zero();
        if (v == 1)
            return one();
        if (v == -1)
            return minus_one();
        if (v == 2)
            return two();

        return Expression::make<Scalar>(v);
    }

    std::ostream& print(std::ostream& out) {    return out << _value; }

    // Helpers
    operator ExprSubType () const { return EST_Scalar; }
    virtual bool is_nul()    {  return _value == 0;   }
    virtual bool is_one()    {  return _value == 1;   }
    virtual bool is_scalar() {  return true;   }
    virtual bool is_leaf()   {  return true;   }
    virtual bool parens()    {  return true;   }

    // Transformation
    SymExpr derivate(const std::string& name){  return zero(); }
    double full_eval(Context& c)             {  return _value;  }
    virtual SymExpr partial_eval(Context& c) {  return Scalar::make(_value); }

    T value() {    return _value; }

private:
    T _value;
};

typedef Scalar<double> ScalarDouble;
typedef Scalar<int> ScalarInt;


// MathConstant are named values
class MathConstant: public ScalarDouble
{
public:
    MathConstant(const std::string& name, double val):
        ScalarDouble(val), _name(name)
    {}

    std::ostream& print(std::ostream& out) {    return out << _name; }

    static SymExpr make(const std::string& name, double v){
        return Expression::make<MathConstant>(name, v);
    }

private:
    std::string _name;
};

}

}

#endif
