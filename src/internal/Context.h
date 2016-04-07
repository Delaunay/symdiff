#ifndef SYMDIF_INTERNAL_CONTEXT_HEADER
#define SYMDIF_INTERNAL_CONTEXT_HEADER

#include <memory>        // shared_ptr
#include <ostream>       // output stream
#include <unordered_map> // Hashtable

namespace symdiff{

    // Forward declaration
    namespace internal{
        class Expression;
        typedef std::shared_ptr<Expression> SymExpr;
    }

    typedef internal::SymExpr SymExpr;
    typedef std::unordered_map<std::string, SymExpr> Context;

    // print context
    std::ostream& operator << (std::ostream& out, Context& ctx);
}

#endif
