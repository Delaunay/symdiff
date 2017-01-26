#ifndef SYMDIFF_INTERNAL_FULLEVAL_HEADER_
#define SYMDIFF_INTERNAL_FULLEVAL_HEADER_

#include "Builder.h"
#include "Nodes.h"
#include "Context.h"

#include <functional>

namespace symdiff{

/*
 *  Using the visitor pattern might actually help me implement parallel
 *  evaluation, as each FullEval instance can be executed in a separate thread
 *  (more or less)
 *
 *  It also enable me to easily implement caching using a std::unordered_map<NodeRef, Result> _mem;
 *
 *  FullEval only works for function that returns numbers.
 *  which is currently the only kind of allowed function.
 *
 *      About Multi threading eval
 *  =================================
 *
 *      You need to be extra careful. If you are evaluating a derivative.
 *
 *          (u * v)' = u' v + u v'
 *
 *      Which means u' and u might have some nodes in common which means
 *      some nodes will be evaluated multiple times.
 *
 *      I propose a global cache for each Eval visitor with a single sync
 *      primitive (for the cache)
 *
 *      // check for value
 *      _cache[node_ptr] == Optional(Empty)
 *
 *      // add cache entry
 *      _cache[node_ptr] = Optional(Pending)
 *
 *      // update cache
 *      _cache[node_ptr] = Optional(Value)
 *
 */
class FullEval: public StaticVisitor<FullEval>
{
public:
    const NameContext& ctx;

    FullEval(const NameContext& ctx):
        ctx(ctx)
    {}

    static double run(const NameContext& ctx, Node expr){
        return FullEval(ctx).eval(expr);
    }

    double eval(Node expr){
        return dispatch<double>(expr);
    }

#define SYMDIFF_NODES_DEFINITIONS
    #define DEFINE_UNARY_NODE(__type__, __str__, __repr__)\
        double __str__ (UnaryNode* u){\
            return Operator::__str__(eval(u->expr));\
        }

    #define DEFINE_BINARY_NODE(__type__, __str__, __repr__)\
        double __str__ (BinaryNode* b){\
            return Operator::__str__(eval(b->lhs), eval(b->rhs));\
        }

    #include "../src/Nodes.def"
#undef SYMDIFF_NODES_DEFINITIONS

    double placeholder(Placeholder* p){
        auto v = ctx.at(p->name);
        return to_value(v)->value;
    }

    double cond(Cond* c) {
        if (eval(c->cond()) > 0)
            return eval(c->texpr());
        else
            return eval(c->fexpr());
    }

    double value(Value* v)      {   return v->value; }
    double catch_all(NodeType ) {   return 0; }
};

}

#endif
