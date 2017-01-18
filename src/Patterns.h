#ifndef SYMDIFF_INTERNAL_PATERNS_HEADER_
#define SYMDIFF_INTERNAL_PATERNS_HEADER_

#include "Nodes.h"
#include "Builder.h"

namespace symdiff
{

/*
 *  A pattern is a regular symdiff expression
 *  but a new node can be used; "Any" which match
 *  with all existing nodes when Any is matched
 *  it saves the node it was matched too
 *
 *  match ((2  + y) * (3 + x)) (_ * _)
 *      first  Any => (2 + y)
 *      Second Any => (3 + x)
 */
inline Node make_pattern_any(){
    return std::make_shared<UnaryNode>(NodeID::Any, zero());
}

/*
 * return after finding at most one matching expression
 */
struct PatternMatcher
{
    PatternMatcher(Node pattern, int max_depth):
        opat(pattern), max_depth(max_depth)
    {}

    static bool run(Node expr, Node pattern, int max_depth = 20){
        return PatternMatcher(pattern, max_depth).match(expr, pattern, 0);
    }

    bool match(const Node& ca, const Node& cb, int depth){
        if (depth > max_depth)
            return false;

        // If AnyNode the pattern is a match
        if (cb->id == NodeID::Any){
            UnaryNode* u = to_unary(cb);
            u->expr = ca;
            return true;
        }

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
                {
                    if (!match(to_unary(ca)->expr, to_unary(cb)->expr, depth))
                        return match(to_unary(ca)->expr, opat, depth + 1);
                    return true;
                    // return match(to_unary(ca)->expr, to_unary(cb)->expr);
                }

                // Binary
        #define SYMDIFF_NODES_DEFINITIONS
            #define DEFINE_BINARY_NODE(__type__, __str__, __repr__)\
                case NodeID::__str__:
            #include "../src/Nodes.def"
        #undef SYMDIFF_NODES_DEFINITIONS
                {
                    BinaryNode* na = to_binary(ca);
                    BinaryNode* nb = to_binary(cb);
                    if (!match(na->lhs, nb->lhs, depth) && match(na->rhs, nb->rhs, depth))
                        return match(na->lhs, opat, depth + 1) ||
                               match(na->rhs, opat, depth + 1);
                    return true;
                    // return match(na->lhs, nb->lhs) && match(na->rhs, nb->rhs);
                }
            case NodeID::Any:
            case NodeID::Size:
                assert("unreachable");
            }
        }

        return false;
    }

    Node opat;
    int max_depth;
};

}

#endif
