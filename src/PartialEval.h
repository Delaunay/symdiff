#ifndef SYMDIFF_INTERNAL_PARTIALEVAL_HEADER_
#define SYMDIFF_INTERNAL_PARTIALEVAL_HEADER_

#include "Builder.h"
#include "Context.h"

#include <functional>

namespace symdiff{

/*
 *  Using the visitor pattern might actually help me implement parallel
 *  evaluation, as each FullEval instance can be executed in a separate thread
 */
class PartialEval: public Visitor
{
  public:

    // const context, variable immutable
    PartialEval(const NameContext& ctx, Node expr):
      result(zero()), ctx(ctx)
    {
        dispatch(expr);
    }

    static Node run(const NameContext& ctx, NodeType expr){
        return PartialEval(ctx, expr).result;
    }

    static Node run(const NodeType expr){
        NameContext ctx;
        return PartialEval(ctx, expr).result;
    }

    void binary_operator(NodeType lhs, NodeType rhs, std::function<Node(Node, Node)>pop,
                                                     std::function<double(double, double)> fop){
        dispatch(lhs);
        Node lhs_expr = result;

        // technically I could implement some kind of shortcut mechanism
        // for operation like mult i.e if (0 * rhs) do not visit rhs

        dispatch(rhs);

        if (is_value(result) && is_value(lhs_expr)){
            double lhs_value = to_value(lhs_expr)->value;
            double rhs_value = to_value(result)->value;

            result = Builder::value(fop(lhs_value, rhs_value));
        }
        else
            result = pop(lhs_expr, result);
    }

    void unary_operator(NodeType expr, std::function<Node(Node)> pop,
                                       std::function<double(double)> fop){
        dispatch(expr);

        if (is_value(result))
            result = Builder::value(fop(to_value(result)->value));
        else
            result = pop(result);
    }

#define SYMDIFF_NODES_DEFINITIONS
    #define DEFINE_UNARY_NODE(__type__, __str__, __repr__)\
        void __str__ (UnaryNode* u) override{\
            unary_operator(u->expr, Builder::__str__, Operator::__str__);\
        }

    #define DEFINE_BINARY_NODE(__type__, __str__, __repr__)\
        void __str__ (BinaryNode* b) override{\
            binary_operator(b->lhs, b->rhs, Builder::__str__, Operator::__str__);\
        }

    #include "../src/Nodes.def"
#undef SYMDIFF_NODES_DEFINITIONS

    void value(Value* v) override {
        result = make_value(v->value);
    }

    void placeholder(Placeholder* p) override {
        if (ctx.count(p->name) == 0)
            result = make_placeholder(p->name);

        else // Node are immutable
          result = const_cast<NodeType>(ctx.at(p->name));
    }

    void cond(Cond* c) override{
        dispatch(c->cond()); Node a = result;

        // can we remove the if
        if (is_value(a)){
            Value* v = to_value(a);
            if (v->value > 0){  dispatch(c->texpr());  }
            else             {  dispatch(c->fexpr());  }
        }
        else{
            dispatch(c->texpr()); Node b = result;
            dispatch(c->fexpr());
            result = make_cond(a, b, result);
        }
    }

    Node result;
    const NameContext& ctx;
};



}

#endif
