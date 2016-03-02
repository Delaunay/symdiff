#ifndef SYMDIF_SYM_HEADER
#define SYMDIF_SYM_HEADER

#include "Expression.h"

namespace symdif{

// Public API
class Sym{
public:

    Sym operator+ (Sym& val){   return SymExpr(new internal::Add(_v, val)); }
    Sym operator* (Sym& val){   return SymExpr(new internal::Mult(_v, val)); }

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
};

Sym make_var(const std::string& v);
Sym make_val(double b);

}

#endif
