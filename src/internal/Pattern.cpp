#include "Pattern.h"

namespace symdiff{
namespace internal{

SymExpr leaf() {
    return Expression::make<LeafPat>();
    //return l;
}

SymExpr unary() {
    return Expression::make<UnaryPat>();
    //return l;
}

SymExpr binary() {
    return Expression::make<BinaryPat>();
    //return l;
}
SymExpr any() {
    return Expression::make<AnyPat>();
    //return l;
}

}
}
