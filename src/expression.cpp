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

    Expr zero(){
        static Expr o = Expr(new Scalar(0));
        return o;
    }

    Expr one(){
        static Expr o = Expr(new Scalar(1));
        return o;
    }

    Expr minus_one(){
        static Expr o = Expr(new Scalar(-1));
        return o;
    }

    Expr Variable::eval(TreeBuilder& tb, Variables& d){
        // if the variable is set return its value
        if (d.count((*this)) == 1)
            return tb.add_node(Expr(new Scalar(d[(*this)])));

        // else the expression is kept
        return Expr(this);
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

    TreeBuilder& TreeBuilder::operator+ (TreeBuilder& b){
        _gc.push_back(_root);
        _root = add(_root, b._root);
        return (*this);
    }

    TreeBuilder& TreeBuilder::operator* (TreeBuilder& b){
        _gc.push_back(_root);
        _root = mult(_root, b._root);
        return (*this);
    }

    TreeBuilder TreeBuilder::operator+ (const TreeBuilder& b) const{
        return TreeBuilder(add(_root, b._root));
    }

    TreeBuilder TreeBuilder::operator* (const TreeBuilder& b) const{
        return TreeBuilder(mult(_root, b._root));
    }

    const TreeBuilder vvar(const std::string& name){
        return TreeBuilder(Expr(new Variable(name)));
    }

    TreeBuilder function(){
        return TreeBuilder(zero());
    }
}
