#include "AbstractExpression.h"
#include "Context.h"

namespace symdiff{

    // print context
    std::ostream& operator << (std::ostream& out, Context& ctx){

    bool fst = true;

    out << "{";

    for (Context::value_type& elem: ctx){
        if (!fst)   out << ", ";    fst = false;

        out << elem.first << ": "; elem.second->print(out);
    }

    return out << "}";
}

}

