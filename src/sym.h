#ifndef SYM_H
#define SYM_H

#include <vector>
//#include <memory>

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
//*
//class Sym
//{
//public:

//    static Sym var(std::string name){
//        return Sym(new Variable(name));
//    }

//    Sym(Expr* v):
//        _root(v)
//    {}

//    Sym derivate(Sym a){
//        return Sym(_root->derivate(a._root));
//    }

//    std::ostream& print(std::ostream& out){
//        _root->print(out);
//        return out;
//    }

//    Sym operator+ (Sym& a, Sym& b){
//        return Sym(add(a._root, b._root));
//    }

//    Sym operator* (Sym& a, Sym& b){
//        return Sym(mult(a._root, b._root));
//    }

//private:

//    Expr* _root;
//    std::vector<Expr*> _gc;
//};

//inline
//std::ostream& operator<< (std::ostream& out, Sym& v){
//    return v.print(out);
//}

//namespace api{
//    TreeBuilder var(std::string& name);
//}



//namespace api{

//    inline
//    const TreeBuilder var(std::string& name){
//        return TreeBuilder(new Variable(name));
//    }

//    inline
//    TreeBuilder function(){
//        return TreeBuilder(zero());
//    }
//}

}

#endif // SYM_H

