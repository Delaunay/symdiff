#include "Placeholder.h"

namespace symdiff {
namespace internal{

const std::string&  get_name(SymExpr v){

    // I doubt switching is more efficient than a 'if(v->is_placeholder())'
    // if it is only over one type

    // but if we switch over n type we are going to make
    // less call to virtual functions (1) instead of (n)
    // (supposing we have implemented n 'is_type()')
    switch(v->get_type()){
        case EST_Placeholder:{
            Placeholder* p = dynamic_cast<Placeholder*>(v.get());
            return p->name();
        }

        default:
            throw CastError("Expression does not hold a name");
    }
}

}
}
