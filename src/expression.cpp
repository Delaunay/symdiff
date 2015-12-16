#include "expression.h"
#include "variables.h"
#include "operators.h"

namespace sym{


    // Exception
    //
    class FullEvalError : public std::exception
    {
    public:
        const char* what() const noexcept{
            return "Unspecified Variable: Can't Fully Evaluate the Expression\n"
                   "\t    - specify the missing variable or\n"
                   "\t    - use eval instead\n";
        }
    };

    Expr* zero(){
        static Scalar z(0);
        return (Expr*) &z;
    }

    Expr* one(){
        static Scalar o(1);
        return (Expr*) &o;
    }

    Expr* minus_one(){
        static Scalar o(-1);
        return (Expr*) &o;
    }

    Expr* Variable::eval(Variables& d){
        // if the variable is set return its value
        if (d.count((*this)) == 1)
            return new Scalar(d[(*this)]);

        // else the expression is kept
        return this;
    }

    Real Variable::full_eval(Variables& d){
        // if the variable is set return its value
        if (d.count((*this)) == 0)
            throw FullEvalError();

        return d[(*this)];
    }

    std::size_t variable_hash::operator() (const Variable& v) const noexcept{
        return _h(v.name());
    }
}
