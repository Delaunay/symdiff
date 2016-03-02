#include "Expression.h"


namespace symdif{
namespace internal {
SymExpr one(){
    static SymExpr o(new ScalarDouble(1));
    return o;
}

SymExpr zero(){
    static SymExpr z(new ScalarDouble(0));
    return z;
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
