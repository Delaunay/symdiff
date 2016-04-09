#ifndef SYMDIF_PLACEHOLDER_HEADER
#define SYMDIF_PLACEHOLDER_HEADER
/*
 *  Description:
 *    - Define an unkown variable that can be replace by any expression
 */

#include "Scalar.h"
#include "Errors.h"

namespace symdiff {
namespace internal{

const std::string&  get_name(SymExpr v);

class Placeholder: public Expression
{
public:
    Placeholder(const std::string& name):
        _name(name)
    {}

    static SymExpr make(const std::string& name){
        return Expression::make<Placeholder>(name);
    }

    SymExpr derivate(const std::string& name) {
        if (name == _name)
            return one();
        return zero();
    }

    // Variable could be hashed by their ptr value instead of their name
    // Why not both ? yeaaaah
    double full_eval(Context& c){
        if (c.find(_name) != c.end())
            return c[_name]->full_eval(c);
        throw FullEvalError("Placeholder variable was not set");
    }

    const std::string& name()               {   return _name;           }
    std::ostream& print(std::ostream& out)  {   return out << _name;    }
    ExprSubType get_type() const            {   return EST_Placeholder; }
    bool is_leaf()                          {   return true;            }
    virtual bool parens()                   {   return true;            }

    SymExpr apply(Context& c){
        if (c.find(_name) != c.end())
            return c[_name];
        return Placeholder::make(_name);
    }

    SymExpr partial_eval(Context& c) {  return apply(c);    }
    SymExpr substitute(Context& c)   {  return apply(c);    }

    bool equal(SymExpr& a) {   return sym_equal(a);     }

    bool sym_equal(SymExpr& a) {

        if (this->get_type() == a->get_type()){
            Placeholder* p = dynamic_cast<Placeholder*>(a.get());
            return p->name() == this->name();
        }

        return false;
    }

    std::string to_string(){ return internal::to_string(get_type()) + ": " + name(); }

private:
    std::string _name;
};

}
}

#endif
