#include "sym.h"
#include "internal/Placeholder.h"
#include "internal/Definitions.h"

#include <unordered_map>

namespace symdiff{

Sym neg(Sym expr)      { return internal::Opposite::make(expr); }
Sym inv(Sym expr)      { return internal::Inverse::make(expr);  }
Sym ln(Sym expr)       { return internal::Ln::make(expr);       }
Sym exp(Sym expr)      { return internal::Exp::make(expr);      }

Sym add(Sym a, Sym b)  { return internal::Add::make(a, b);     }
Sym mult(Sym a, Sym b) { return internal::Mult::make(a, b);    }

Sym sub(Sym a, Sym b)  { return add(a, neg(b));      }
Sym div(Sym a, Sym b)  { return mult(a, inv(b));     }
Sym pow(Sym a, Sym b)  { return internal::Pow::make(a, b);} //return exp(mult(b, ln(a))); }

Sym minus_one()  { return Sym(internal::minus_one()); }
Sym zero()       { return Sym(internal::zero()); }
Sym one()        { return Sym(internal::one()); }
Sym two()        { return Sym(internal::two()); }
Sym e()          { return Sym(internal::e()); }
Sym pi()         { return Sym(internal::pi()); }

Sym leaf()      {   return Sym(internal::leaf()); }
Sym unary()     {   return Sym(internal::unary()); }
Sym binary()    {   return Sym(internal::binary()); }
Sym any()       {   return Sym(internal::any()); }

Sym make_var(const std::string& v) {
    return internal::Placeholder::make(v);
}

Sym make_val(const std::string& name, double v){
    return internal::MathConstant::make(name, v);
}

Sym make_val(double b){
    return internal::ScalarDouble::make(b);
}

Sym make_global(const std::string& v){
    static std::unordered_map<std::string, Sym> _val;

    if (_val.find(v) == _val.end())
        _val[v] = make_var(v);

    return _val[v];
}

Sym make_global_val(const std::string& v, double val){
    static std::unordered_map<std::string, Sym> _val;

    if (_val.find(v) == _val.end())
        _val[v] = make_val(v, val);

    return _val[v];
}

}
