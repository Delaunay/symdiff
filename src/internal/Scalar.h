#ifndef SYMDIF_INTERNAL_SCALAR_HEADER
#define SYMDIF_INTERNAL_SCALAR_HEADER
/*
 *  Description:
 *    - Define a Constant Value
 */

#include "AbstractExpression.h"
#include <memory>

#ifdef USE_LLVM_IR
#   include "llvm/ADT/APFloat.h"
#endif

namespace symdiff {
namespace internal{

// retrieve a value from a Scalar
double get_value(SymExpr v);

// Pre allocate common variables
// not really to save memory but rather to save two allocation
// one for the counter one for the expression
SymExpr& minus_one();
SymExpr& zero();
SymExpr& one();
SymExpr& two();
SymExpr& pi();
SymExpr& e();

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
    std::ostream& gen(std::ostream& out, OutputType t) { return out << _value; }

    // Helpers
    ExprSubType get_type() const { return EST_Scalar; }
    virtual bool is_nul()    {  return _value == 0;   }
    virtual bool is_one()    {  return _value == 1;   }
    virtual bool is_scalar() {  return true;   }
    virtual bool is_leaf()   {  return true;   }
    virtual bool parens()    {  return false;   }
            int  height(int i = 0) {  return i;   }

    // Transformations
    SymExpr derivate(const std::string& name){  return zero(); }
    double full_eval(Context& c)             {  return _value;  }

    double full_eval(PtrContext& c)     {  return _value;  }
    SymExpr partial_eval(PtrContext& c) {
        // we lose old address
        return Scalar::make(_value);
    }

    // Apply Function
    SymExpr partial_eval(Context& c) {  return Scalar::make(_value); }
    SymExpr substitute  (Context& c) {  return Scalar::make(_value); }

    T value() {    return _value; }

    bool sym_equal(SymExpr& a) {

        if (this->get_type() == a->get_type()){
            Scalar<T>* p = dynamic_cast<Scalar<T>*>(a.get());
            return p->value() == this->value();
        }

        if (a->is_pattern())
            return pattern_equal(a);

        return false;
    }

    bool equal(SymExpr& a) {   return sym_equal(a);     }

    bool pattern_equal(SymExpr& a) { return a->is_leaf();   }

    std::string to_string(){ return internal::to_string(get_type()) + ": " + std::to_string(value()); }

#ifdef USE_LLVM_IR
    virtual llvm::Value* llvm_gen(llvm::IRBuilder<>& bl, ArgIterator, int&){
        //return llvm::ConstantFP::get(bl.getContext(), llvm::APFloat(_value));
        return llvm::ConstantFP::get(bl.getDoubleTy(), _value);
    }
#endif

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
    std::ostream& gen(std::ostream& out, OutputType t) { return out << _name; }

    static SymExpr make(const std::string& name, double v){
        return Expression::make<MathConstant>(name, v);
    }

private:
    std::string _name;
};

}

}

#endif
