#ifndef SYMDIF_SYM_HEADER
#define SYMDIF_SYM_HEADER

#include "internal/Placeholder.h"
#include "internal/Definitions.h"

namespace symdiff{

// Public API

class Sym;

Sym make_val(double b);

class Sym{
public:
    static Sym neg(Sym expr)      { return internal::Opposite::make(expr); }
    static Sym inv(Sym expr)      { return internal::Inverse::make(expr);  }
    static Sym ln(Sym expr)       { return internal::Ln::make(expr);       }
    static Sym exp(Sym expr)      { return internal::Exp::make(expr);      }

    static Sym add(Sym a, Sym b)  { return internal::Add::make(a, b);     }
    static Sym mult(Sym a, Sym b) { return internal::Mult::make(a, b);    }

    static Sym sub(Sym a, Sym b)  { return add(a, neg(b));      }
    static Sym div(Sym a, Sym b)  { return mult(a, inv(b));     }
    static Sym pow(Sym a, Sym b)  { return exp(mult(b, ln(a))); }

    static Sym minus_one()  { return Sym(internal::minus_one()); }
    static Sym zero()       { return Sym(internal::zero()); }
    static Sym one()        { return Sym(internal::one()); }
    static Sym two()        { return Sym(internal::two()); }
    static Sym e()          { return Sym(internal::e()); }
    static Sym pi()         { return Sym(internal::pi()); }

    Sym operator- ()        {   return neg(_v);       }


    // Allow operation with Numeric type that can be cast to a double
    Sym operator+ (double v){ return add(_v, make_val(v));  }
    Sym operator* (double v){ return mult(_v, make_val(v)); }
    Sym operator- (double v){ return sub(_v, make_val(v));  }
    Sym operator/ (double v){ return div(_v, make_val(v));  }
    Sym operator^ (double v){ return pow(_v, make_val(v));  }

    Sym operator+ (Sym val){   return add(_v, val);  }
    Sym operator* (Sym val){   return mult(_v, val); }
    Sym operator- (Sym val){   return sub(_v, val);  }
    Sym operator/ (Sym val){   return div(_v, val);  }
    Sym operator^ (Sym val){   return pow(_v, val);  }

    std::ostream& print(std::ostream& out) {    return _v->print(out);      }
    Sym derivate(const std::string& name)  {    return _v->derivate(name);  }

    double full_eval(Context& c) {    return _v->full_eval(c);     }
    Sym partial_eval(Context& c) {    return _v->partial_eval(c); }

    // Implicit cast to SymExpr
    operator SymExpr(){  return _v; }

private:
    Sym(SymExpr v):
        _v(v)
    {}

    internal::SymExpr _v;

    friend Sym make_var(const std::string &v);
    friend Sym make_val(double b);
    friend Sym make_named_val(const std::string& v, double b);
};


Sym make_var(const std::string& v);
Sym make_named_val(const std::string& v, double b);

/*
template<typename T, typename... Args>
static SymExpr make(Args&&... args){
    return std::make_shared<T>(std::forward<Args>(args)...);
} */

typedef std::pair<std::string, Sym> Arg;

template<typename T, typename... Args>
void build_ctx(Context& a,T arg1, Args&&... args){
    a[arg1.first] = arg1.second;

    return buid_ctx(a, std::forward<Arg>(args)...);
}

template<typename T>
void build_ctx(Context& a, T arg1){
    a[arg1.first] = arg1.second;
}

template<typename... Args>
Sym call(Sym function, Args&&... args){
    Context c;  build_ctx(c, std::forward<Args>(args)...);
    return function.partial_eval(c);
}

template<typename... Args>
Sym partial_call(Sym function, Args&&... args){
    Context c;  build_ctx(c, std::forward<Args>(args)...);
    return function.partial_eval(c);
}
template<typename... Args>
double full_call(Sym function, Args&&... args){
    Context c;  build_ctx(c, std::forward<Args>(args)...);
    return function.full_eval(c);
}

//  Shortcuts
// -------------------------------



}

#endif
