#ifndef SYMDIF_SYM_HEADER
#define SYMDIF_SYM_HEADER

#include "internal/AbstractExpression.h"
#include "internal/Utils.h"

namespace symdiff{

// Public API
class Sym;

Sym make_val(double b);
Sym neg(Sym expr);
Sym inv(Sym expr);
Sym ln(Sym expr);
Sym exp(Sym expr);

Sym add(Sym a, Sym b);
Sym mult(Sym a, Sym b);

Sym sub(Sym a, Sym b);
Sym div(Sym a, Sym b);
Sym pow(Sym a, Sym b);

Sym minus_one();
Sym zero();
Sym one();
Sym two();
Sym e();
Sym pi();

// Hollow class
class Sym{
public:
    Sym operator- ()        {   return neg(_v);       }

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

    bool       equal(const Sym& a) const { return _v->equal(a._v);    }
    bool operator== (const Sym& b) const { return this->equal(b);     }

    std::ostream& gen(std::ostream& out, OutputType t) {    return _v->gen(out, t);      }
    std::ostream& print(std::ostream& out) {    return _v->print(out);      }
    Sym derivate(const std::string& name)  {    return _v->derivate(name);  }

    double full_eval(Context& c) {  return _v->full_eval(c);    }
    Sym partial_eval(Context& c) {  return _v->partial_eval(c); }
    double full_eval(PtrContext& c) {  return _v->full_eval(c);    }
    Sym partial_eval(PtrContext& c) {  return _v->partial_eval(c); }
    Sym substitute(Context& c)   {  return _v->substitute(c);   }
    Sym eval(Context c)          {  return _v->partial_eval(c); }
    Sym reduce()      {  Context c; return _v->partial_eval(c); }

    // Implicit cast to SymExpr
    // needed because Context use SymExpr i.e make_val must be converted to SymExpr
    operator SymExpr(){  return _v; }

    // required for unordered_map
    Sym() {}

    // Implicit conversion double -> Sym
    Sym(double v):
        _v(make_val(v))
    {}

private:
    // force everything public to be Sym
    // makes sure the API does not return SymExpr.
    // Not sure if it is such a great idea
    Sym(SymExpr v):
        _v(v)
    {}

    mutable internal::SymExpr _v;

    friend Sym make_var(const std::string &v);
    friend Sym make_val(double b);
    friend Sym make_val(const std::string& v, double b);
    friend Sym make_global_val(const std::string& v, double val);
    friend Sym make_global(const std::string& v);

    friend Sym neg(Sym expr);
    friend Sym inv(Sym expr);
    friend Sym ln(Sym expr);
    friend Sym exp(Sym expr);

    friend Sym add(Sym a, Sym b);
    friend Sym mult(Sym a, Sym b);

    friend Sym sub(Sym a, Sym b);
    friend Sym div(Sym a, Sym b);
    friend Sym pow(Sym a, Sym b);

    friend Sym minus_one();
    friend Sym zero();
    friend Sym one();
    friend Sym two();
    friend Sym e();
    friend Sym pi();
};

// implicit => make_local
Sym make_var(const std::string& v);
Sym make_val(const std::string& v, double b);

// make global keep tracks of defined variables
// if the variable already exists its ptr is returned
// instead of creating another one
Sym make_global(const std::string& v);
Sym make_global_val(const std::string& v, double val);

typedef std::pair<std::string, Sym> Arg;

template<typename T>
void build_ctx(Context& a, T arg1){
    a[arg1.first] = arg1.second;
}

template<typename T, typename... Args>
void build_ctx(Context& a,T arg1, Args... args){
    a[arg1.first] = arg1.second;
    build_ctx(a, args...);
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

inline Sym call(Sym function, Context c){
    return function.partial_eval(c);
}

inline Sym partial_call(Sym function, Context c){
    return function.partial_eval(c);
}

inline double full_call(Sym function, Context c){
    return function.full_eval(c);
}


}

#endif
