#include "Utils.h"

namespace symdiff{
namespace internal{

void reorder(SymExpr& a, SymExpr& b){

    ExprSubType ta = a->get_type();
    ExprSubType tb = b->get_type();

    // nothing to do
    if (ta <= tb)
        return;

    // (1 * 0) => (0 * 1)
//    if (ta == EST_Scalar && ta == tb){
//        if (get_value(a) < get_value(b))
//            return;
//    }

    std::swap(a, b);
}

}
}
