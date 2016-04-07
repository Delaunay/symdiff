#include "sym.h"

namespace symdiff{

Sym make_var(const std::string& v) {
    return internal::Placeholder::make(v);
}

Sym make_val(double v){
    return internal::ScalarDouble::make(v);
}

}
