#include "sym.h"

namespace symdif{

Sym make_var(const std::string& v) {
    return SymExpr(new internal::Placeholder(v));
}

Sym make_val(double v){
    return SymExpr(new internal::ScalarDouble(v));
}

}
