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
class FullEval: public Visitor
{
public:
    double result;
    const NameContext& ctx;

    FullEval(const NameContext& ctx, Node expr):
        ctx(ctx)
    {
        dispatch(expr);
    }

    static double run(const NameContext& ctx, Node expr){
        return FullEval(ctx, expr).result;
    }

    void binary_operator(NodeType lhs, NodeType rhs, std::function<double(double, double)> op){
        dispatch(lhs);
        double lhs_val = result;

        dispatch(rhs);
        result = op(lhs_val, result);
    }

    void unary_operator(NodeType expr, std::function<double(double)> op){
        dispatch(expr);
        result = op(result);
    }

#define SYMDIFF_NODES_DEFINITIONS
    #define DEFINE_UNARY_NODE(__type__, __str__, __repr__)\
        void __str__ (UnaryNode* u) override{\
            unary_operator(u->expr,Operator::__str__);\
        }

    #define DEFINE_BINARY_NODE(__type__, __str__, __repr__)\
        void __str__ (BinaryNode* b) override{\
            binary_operator(b->lhs, b->rhs, Operator::__str__);\
        }

    #include "../src/Nodes.def"
#undef SYMDIFF_NODES_DEFINITIONS

    void value(Value* v) override{
        result = v->value;
    }

    void placeholder(Placeholder* p) override{
        auto v = ctx.at(p->name);
        result = to_value(v)->value;
    }

    void cond(Cond* c) override{
        dispatch(c->cond());

        if (result > 0)
            dispatch(c->texpr());
        else
            dispatch(c->fexpr());
    }
};

}

#endif
