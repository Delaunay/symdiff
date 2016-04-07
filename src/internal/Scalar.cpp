#include "Scalar.h"
#include "Errors.h"


namespace symdiff {
namespace internal {

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


SymExpr pi(){
    static SymExpr o = Expression::make<MathConstant>("pi", 3.141592653589793);
    return o;
}

SymExpr e(){
    static SymExpr o = Expression::make<MathConstant>("e", 2.718281828459045);
    return o;
}


double get_value(SymExpr v){
    if (v->is_scalar()){
        ScalarDouble* e = dynamic_cast<ScalarDouble*>(v.get());
        return e->value();
    }
    throw CastError("Expression does not hold a value");
}

}
}
