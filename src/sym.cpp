#include "sym.h"

namespace symdiff{

Sym make_var(const std::string& v) {
    return internal::Placeholder::make(v);
}

Sym make_named_val(const std::string& name, double v){
    return internal::MathConstant::make(name, v);
}

Sym make_val(double b){
    return internal::ScalarDouble::make(b);
}


}
