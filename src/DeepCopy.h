#ifndef SYMDIFF_INTERNAL_DEEPCOPY_HEADER_
#define SYMDIFF_INTERNAL_DEEPCOPY_HEADER_

#include "Builder.h"
#include "Context.h"

#include <functional>

namespace symdiff{

class DeepCopy: public Visitor
{
  public:
    // Lazy version for children (^_^)
    DeepCopy():
        result(zero())
    {}

    DeepCopy(Node expr):
        result(zero())
    {
        dispatch(expr);
    }

    static Node run(NodeType expr){
        return DeepCopy(expr).result;
    }

    void binary_operator(NodeType lhs, NodeType rhs, std::function<Node(Node, Node)>pop){
        dispatch(lhs);
        Node lhs_expr = result;
        dispatch(rhs);
        result = pop(lhs_expr, result);
    }

    void unary_operator(NodeType expr, std::function<Node(Node)> pop){
        dispatch(expr);
        result = pop(result);
    }

#define SYMDIFF_NODES_DEFINITIONS
    #define DEFINE_UNARY_NODE(__type__, __str__, __repr__)\
        void __str__ (UnaryNode* u) override{\
            return unary_operator(u->expr, Builder::__str__);\
        }

    #define DEFINE_BINARY_NODE(__type__, __str__, __repr__)\
        void __str__ (BinaryNode* b) override{\
            return binary_operator(b->lhs, b->rhs, Builder::__str__);\
        }

    #include "../src/Nodes.def"
#undef SYMDIFF_NODES_DEFINITIONS


    void value(Value* s) override {
        result = Builder::value(s->value);
    }

    void placeholder(Placeholder* p) override {
        result = Builder::placeholder(p->name);
    }

    void cond(Cond* c) override{
        dispatch(c->cond());  Node a = result;
        dispatch(c->texpr()); Node b = result;
        dispatch(c->fexpr());
        result = make_cond(a, b, result);
    }

    Node result;
};

}

#endif
