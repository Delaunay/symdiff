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


    /// std::hash specialization for shared_ptr.
    template<typename T>
    struct shared_ptr_hash: public std::hash<std::shared_ptr<T>>
    {
        size_t operator()(const std::shared_ptr<T>& s) const noexcept
        {
            return std::hash<T*>()(s.get());
        }

        size_t operator()(T* const & s) const noexcept
        {
            return std::hash<T*>()(s);
        }
    };

    // Ptr Context
    // auto x = make_var("x");
    // PtrContext ctx = { {x: make_val(0.25)} };

    typedef std::unordered_map<internal::Expression*, SymExpr,
                shared_ptr_hash<internal::Expression>> PtrContext;



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

    // ++ Pattern identification


}

#endif
