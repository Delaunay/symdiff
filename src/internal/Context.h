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

    // Name Context
    // auto x = make_var("x");
    // NameContext ctx = { {"x": make_val(0.25)} };
    typedef internal::SymExpr SymExpr;
    typedef std::unordered_map<std::string, SymExpr> NameContext;
    typedef NameContext Context;

    // print context
    std::ostream& operator << (std::ostream& out, Context& ctx);


    // Ptr Context
    // auto x = make_var("x");
    // PtrContext ctx = { {x: make_val(0.25)} };
    // typedef std::unordered_map<SymExpr, SymExpr> NameContext;

    // SymExpr is hashed by ptr value
    // This might be even better Than I imagined

    //
    // phi = x * x;
    // y = phi * phi;
    //
    // ctx = {{phi: 2}}
    //
    // y.substitute(ctx) => 2 * 2
    //

    // auto x = make_var(...
    //
    // {
    //      auto x = make_var(...
    //
    //          inside expr x != x
    // }


}

#endif
