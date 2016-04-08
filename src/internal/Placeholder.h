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

    double full_eval(Context& c){
        if (c.find(_name) != c.end())
            return c[_name]->full_eval(c);
        throw FullEvalError("Placeholder variable was not set");
    }

    SymExpr partial_eval(Context& c) {
        if (c.find(_name) != c.end())
            return c[_name];
        return Placeholder::make(_name);
    }

    const std::string& name()               {   return _name;           }
    std::ostream& print(std::ostream& out)  {   return out << _name;    }
    operator ExprSubType () const           {   return EST_Placeholder; }
    bool is_leaf()                          {   return true;            }
    virtual bool parens()                   {   return true;            }

private:
    std::string _name;
};

}
}

#endif
