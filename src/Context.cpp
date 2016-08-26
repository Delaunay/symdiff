#include "PrettyPrint.h"
#include "Context.h"

namespace symdiff{

// print context
std::ostream& operator << (std::ostream& out, NameContext& ctx){

    bool fst = true;

    out << "{";

    for (NameContext::value_type& elem: ctx){
        if (!fst)
            out << ", ";    fst = false;

        out << elem.first << ": ";
        PrettyPrint::run(out, elem.second);
    }

    return out << "}";
}

}

