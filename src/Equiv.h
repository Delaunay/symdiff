#ifndef SYMDIFF_INTERNAL_EQUIV_HEADER_
#define SYMDIFF_INTERNAL_EQUIV_HEADER_

#include "Nodes.h"

#include <cassert>
#include <cmath>

namespace symdiff{

/*
 *  Equality checking
 */

bool eq_sym(const Node& ca, const Node& cb){

    if (ca->id == cb->id){

        switch(ca->id){
            // Leaf
            case NodeID::placeholder:
                return to_placeholder(ca)->name == to_placeholder(cb)->name;

            case NodeID::value:
                return std::abs(to_value(ca)->value - to_value(cb)->value) < 1e-12;

            // Unary
    #define SYMDIFF_NODES_DEFINITIONS
        #define DEFINE_UNARY_NODE(__type__, __str__, __repr__)\
            case NodeID::__str__:
        #include "../src/Nodes.def"
    #undef SYMDIFF_NODES_DEFINITIONS

                return eq_sym(to_unary(ca)->expr, to_unary(cb)->expr);

            // Binary
    #define SYMDIFF_NODES_DEFINITIONS
        #define DEFINE_BINARY_NODE(__type__, __str__, __repr__)\
            case NodeID::__str__:
        #include "../src/Nodes.def"
    #undef SYMDIFF_NODES_DEFINITIONS
            {
                BinaryNode* na = to_binary(ca);
                BinaryNode* nb = to_binary(cb);
                return eq_sym(na->lhs, nb->lhs) && eq_sym(na->rhs, nb->rhs);
            }
        case NodeID::Size:
            assert("unreachable");
        }
    }

    return false;
}

bool eq_mem(const Node& ca, const Node& cb){
    return ca.get() == cb.get();
}

bool equiv(const Node& ca, const Node& cb){
    return eq_mem(ca, cb) || eq_sym(ca, cb);
}

}

#endif
