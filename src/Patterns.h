#ifndef SYMDIFF_INTERNAL_PATERNS_HEADER_
#define SYMDIFF_INTERNAL_PATERNS_HEADER_

#include "Nodes.h"
#include "Builder.h"

namespace symdiff
{

/*
 *  A pattern is a regular symdiff expression
 *  but a new node can be used; "Any"
 *  when Any is matched
 *  it saves the node it was matched too
 *
 *  match ((2  + y) * (3 + x)) (_ * _)
 *      first  Any ref to (2 + y)
 *      Second Any ref to (3 + x)
 */
inline Node make_pattern_any(){
    return std::make_shared<UnaryNode>(NodeID::Any, zero());
}

struct PatternMatcher: public StaticVisitor<PatternMatcher>
{
    static int& max_depth() {
        static int mx = 255;
        return mx;
    }

    PatternMatcher(Node opat):
        opat(opat)
    {}

    static bool run(Node a, Node b){
        return PatternMatcher(b).match(a, b, 0);
    }

    bool match(Node a, Node b, int depth, bool pleaf=false){
        // If AnyNode the pattern is a match
        if (b->id == NodeID::Any){
            UnaryNode* u = to_unary(b);
            u->expr = a;
            return true;
        }

        if (depth > max_depth())
            return false;

        if (a->id == b->id)
            return dispatch<bool>(a, b, depth, pleaf);
        return false;
    }

#define SYMDIFF_NODES_DEFINITIONS
    #define DEFINE_UNARY_NODE(__type__, __str__, __repr__)\
        bool __str__ (UnaryNode* a, Node b, int depth, bool pleaf=false){\
            if (!match(a->expr, to_unary(b)->expr, depth, true))\
                return match(a->expr, opat, depth + 1, false);\
            return true;\
        }

    #define DEFINE_BINARY_NODE(__type__, __str__, __repr__)\
        bool __str__ (BinaryNode* a, Node b, int depth, bool pleaf=false){\
            BinaryNode* nb = to_binary(b);\
            if (!(match(a->lhs, nb->lhs, depth) && match(a->rhs, nb->rhs, depth, true)))\
                return match(a->lhs, opat, depth + 1, false) ||\
                       match(a->rhs, opat, depth + 1, false);\
            return true;\
        }

    #include "../src/Nodes.def"
#undef SYMDIFF_NODES_DEFINITIONS

    bool cond(Cond* a, Node b, int depth, bool pleaf=false) {
        NnaryNode* ca = reinterpret_cast<NnaryNode*>(a);
        NnaryNode* cb = to_nnary(b);

        if (ca->size() != cb->size())
            return false;

        // lookup children patterns in children nodes
        for(std::size_t i = 0; i < ca->size(); ++i){
            if (match((*ca)[i], (*cb)[i], depth, pleaf)){
                return true;
            }
        }

        // lookup root pattern in children nodes
        for(std::size_t i = 0; i < ca->size(); ++i){
            if (match((*ca)[i], opat, depth + 1, false))
                return true;
        }

        return false;
    }

    bool placeholder(Placeholder* p, Node b, int, bool){
         return p->name == to_placeholder(b)->name;
    }

    bool value(Value* v, Node b, int, bool){
        return std::abs(v->value - to_value(b)->value) < 1e-12;
    }

    bool catch_all(Node, Node, int, bool) { return false; }

    Node opat; // pattern root
};


struct Equality: public StaticVisitor<Equality>
{
    static bool run(Node a, Node b){
        return Equality().equal(a, b);
    }

    bool equal(Node a, Node b){
        if (a->id == b->id)
            return dispatch<bool>(a, b);
        return false;
    }

#define SYMDIFF_NODES_DEFINITIONS
    #define DEFINE_UNARY_NODE(__type__, __str__, __repr__)\
        bool __str__ (UnaryNode* a, Node b){\
            return equal(a->expr, to_unary(b)->expr);\
        }

    #define DEFINE_BINARY_NODE(__type__, __str__, __repr__)\
        bool __str__ (BinaryNode* a, Node b){\
            BinaryNode* nb = to_binary(b);\
            return equal(a->lhs, nb->lhs) && equal(a->rhs, nb->lhs);\
        }

    #include "../src/Nodes.def"
#undef SYMDIFF_NODES_DEFINITIONS

    bool placeholder(Placeholder* p, Node b){
         return p->name == to_placeholder(b)->name;
    }

    bool cond(Cond* a, Node b) {
        NnaryNode* ca = reinterpret_cast<NnaryNode*>(a);
        NnaryNode* cb = to_nnary(b);

        if (ca->size() != cb->size())
            return false;

        for(std::size_t i = 0; i < ca->size(); ++i){
            if (!equal((*ca)[i], (*cb)[i]))
                return false;
        }
        return true;
    }

    bool value(Value* v, Node b){
        return std::abs(v->value - to_value(b)->value) < 1e-12;
    }

    bool catch_all(Node, Node) { return false;}

};


///*
// * return after finding at most one matching expression
// */
//struct PatternMatcher
//{
//    PatternMatcher(Node pattern, int max_depth):
//        opat(pattern), max_depth(max_depth)
//    {}

//    static bool run(Node expr, Node pattern, int max_depth = 20){
//        return PatternMatcher(pattern, max_depth).match(expr, pattern, 0);
//    }
//    /*
//     * ca   : expression
//     * cb   : Pattern
//     * depth: times we restarted looking for the pattern in the tree leaves
//     * pleaf: are we checking a leaf of pattern
//     * /
//    bool match(const Node& ca, const Node& cb, int depth, bool pleaf=false){
//        if (depth > max_depth)
//            return false;

//        // If AnyNode the pattern is a match
//        if (cb->id == NodeID::Any){
//            UnaryNode* u = to_unary(cb);
//            u->expr = ca;
//            return true;
//        }

//        if (ca->id == cb->id){
//            switch(ca->id){
//                // Leaf
//                case NodeID::placeholder:
//                {
//                    /*/ Seek'n replace
//                    if (to_placeholder(ca)->name == to_placeholder(cb)->name){
//                        if (!pleaf) ca = replace_expr;
//                        return true;
//                    }
//                    i
//                    return false;
//                    //*/

//                    return to_placeholder(ca)->name == to_placeholder(cb)->name;
//                }

//                case NodeID::value:
//                {
//                    /*/ Seek'n replace
//                    if (std::abs(to_value(ca)->value - to_value(cb)->value) < 1e-12){
//                        if (!pleaf) ca = replace_expr;
//                        return true;
//                    }
//                    return false;
//                    //*/

//                    return std::abs(to_value(ca)->value - to_value(cb)->value) < 1e-12;
//                }
//                // Unary
//        #define SYMDIFF_NODES_DEFINITIONS
//            #define DEFINE_UNARY_NODE(__type__, __str__, __repr__)\
//                case NodeID::__str__:
//            #include "../src/Nodes.def"
//        #undef SYMDIFF_NODES_DEFINITIONS
//                {
//                    /*/ Seek'n replace
//                    if (match(to_unary(ca)->expr, to_unary(cb)->expr, depth, true)){
//                        if (!pleaf) ca = replace_expr;
//                        return true;
//                    }
//                    else if (match(to_unary(ca)->expr, opat, depth + 1, false)){
//                        to_unary(ca)->expr = replace_expr;
//                        return true;
//                    }
//                    return false;
//                    //*/

//                    if (!match(to_unary(ca)->expr, to_unary(cb)->expr, depth, true))
//                        return match(to_unary(ca)->expr, opat, depth + 1, false);
//                    return true;

//                    // depth 1
//                    // return match(to_unary(ca)->expr, to_unary(cb)->expr);
//                }

//                // Binary
//        #define SYMDIFF_NODES_DEFINITIONS
//            #define DEFINE_BINARY_NODE(__type__, __str__, __repr__)\
//                case NodeID::__str__:
//            #include "../src/Nodes.def"
//        #undef SYMDIFF_NODES_DEFINITIONS
//                {
//                    BinaryNode* na = to_binary(ca);
//                    BinaryNode* nb = to_binary(cb);

//                    /*/ Seek'n replace
//                    if (match(na->lhs, nb->lhs, depth) && match(na->rhs, nb->rhs, depth, true)){
//                        if (!pleaf) ca = replace_expr;
//                        return true;
//                    }
//                    else if (match(na->lhs, opat, depth + 1, false)){
//                        na->lhs = replace_expr;
//                        return true;
//                    }
//                    else if (match(na->rhs, opat, depth + 1, false)){
//                        na->rhs = replace_expr;
//                        return true;
//                    }
//                    return false;
//                    //*/

//                    if (!match(na->lhs, nb->lhs, depth) && match(na->rhs, nb->rhs, depth, true))
//                        return match(na->lhs, opat, depth + 1, false) ||
//                               match(na->rhs, opat, depth + 1, false);
//                    return true;
//                    // return match(na->lhs, nb->lhs) && match(na->rhs, nb->rhs);
//                }
//            case NodeID::cond:
//            {
//                return false;
//            }
//            case NodeID::Any:
//            case NodeID::Size:
//                assert("unreachable");
//            }
//        }

//        return false;
//    }

//    Node opat;
//    // Node replace_expr;
//    int max_depth;
//};

}

#endif
