#include "Utils.h"
#include "Placeholder.h"

#include <list>

namespace symdiff{
namespace internal{

void reorder(SymExpr& a, SymExpr& b){

    ExprSubType ta = a->get_type();
    ExprSubType tb = b->get_type();

    // nothing to do
    if (ta < tb)
        return;

    if (ta == tb){
        if (ta == EST_Placeholder && get_name(a) < get_name(b))
            return;

        // if reordering for scalar nodes is disabled
        // this can cause equiv to fail
        // but fixing this will cause some simplification to not occur
        // Addition and mult collapsing.
        // They do not occur iff they are composed entirely if Scalar nodes
        // which happen from time to time

        // Scalar nodes reordering enabled
        if (ta == EST_Scalar && get_value(a) < get_value(b))
            return;
    }

    std::swap(a, b);
}


// the easy way is to flatten the graph and compare equality one by one
// I am wondering if a vector would be better or not
// This function is not used anymore anyway
// (a * b) + c => (+ c (* a b)) => flatten (+ c * a b)

// Note:: list is reversed
 std::list<SymExpr> flatten(SymExpr& a){

    std::list<SymExpr> r = {a};

    if (a->is_leaf())
        return r;

    if (a->is_binary()){
        BinaryOperator* b = dynamic_cast<BinaryOperator*>(a.get());

        auto x = flatten(b->lhs());
        r.insert(r.begin(), x.begin(), x.end());

        auto y = flatten(b->rhs());
        r.insert(r.begin(), y.begin(), y.end());

        return r;
    }
    else{
        UnaryOperator* u = dynamic_cast<UnaryOperator*>(a.get());

        auto y = flatten(u->expr());
        r.insert(r.begin(), y.begin(), y.end());

        return r;
    }
}

// Symbolic equal   (same tree structure)
bool eq_sym(SymExpr& a, SymExpr& b) {
    // This is lazy because (a && b)
    return a->equal(b);

    /*
    auto la = flatten(a);
    auto lb = flatten(b);

    if (la.size() != lb.size())
        return false;

    {
        auto it = la.begin();
        auto jt = lb.begin();

        // same size
        while(it != la.end()){

            if (!(*it)->sym_equal(*jt))
                return false;

            ++it;
            ++jt;
        }

        return true;

    }
    return false; */
}

}
}
