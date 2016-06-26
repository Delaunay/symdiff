#include "AbstractExpression.h"

namespace symdiff{
namespace internal{

std::string to_string(ExprSubType t){
    switch(t){
        // Leafs
        case EST_Scalar     : return "Scalar";
        case EST_Placeholder: return "Placeholder";

        // Nodes::Binary
        case EST_Add        : return "Add";
        case EST_Mult       : return "Mult";
        case EST_Pow        : return "Pow";

        // Nodes::Unary
        case EST_Sub        : return "Sub";
        case EST_Neg        : return "Neg";
        case EST_Inv        : return "Inv";
        case EST_Exp        : return "Exp";
        case EST_Ln         : return "Ln";
        default             : return "Not a Node";
    }
}

void Expression::free_variables(std::vector<string_view>&) {}

}
}
