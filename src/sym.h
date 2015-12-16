#ifndef SYM_H
#define SYM_H

//#include <vector>
#include <memory>

#include "operators.h"

// public API
namespace sym{

/*
 *  Sym x = var(x);
 *
 *  Sym f = x * x * x;
 *
 *  Sym df = f.derivate(x);
 */

//
//  Symbol
//
//      This is the public interface that should guarantee no memory leaks
/*
class Sym
{
public:

    static Sym var(std::string name){
        return Sym(new Variable(name));
    }

    Sym operator* (Sym a){
        return Sym(mult(this->_root.get(), a._root.get()));
    }

    Sym operator+ (Sym a){
        return Sym(add(this->_root.get(), a._root.get()));
    }

    Sym derivate(Sym a){
        return Sym(_root.get()->derivate(a._root.get()));
    }

    std::ostream& print(std::ostream& out){
        _root.get()->print(out);
        return out;
    }

private:

    Sym(Expr* v):
        _root(v)
    {}

    std::shared_ptr<Expr> _root;
};

inline
std::ostream& operator<< (std::ostream& out, Sym& v){
    return v.print(out);
}//*/

}

#endif // SYM_H

