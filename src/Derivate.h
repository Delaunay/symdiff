#ifndef SYMDIFF_INTERNAL_DERIVATE_HEADER_
#define SYMDIFF_INTERNAL_DERIVATE_HEADER_

#include "Nodes.h"
#include "Builder.h"

//#include "Context.h"

namespace symdiff{

class Derivate: public Visitor
{
  public:

    // const context, variable immutable
    Derivate(const std::string& name, NodeType expr):
      result(zero()), variable(name)
    {
        dispatch(expr);
    }

    static Node run(const std::string& ctx, NodeType expr){
        return Derivate(ctx, expr).result;
    }

    void add(NodeType v) override {
        BinaryNode* b = to_binary(v);

        NodeType rhs = b->rhs;
        NodeType lhs = b->lhs;

        dispatch(lhs);
        Node dlhs = result;

        dispatch(rhs);
        result = Builder::add(dlhs, result);
    }

//    void sub(NodeType lhs, NodeType rhs){
//        dispatch(lhs);
//        NodeType dlhs = result;

//        dispatch(rhs);
//        result = Builder::sub(dlhs, result);
//    }

    void mult(NodeType v) override {
        BinaryNode* b = to_binary(v);

        NodeType rhs = b->rhs;
        NodeType lhs = b->lhs;

        dispatch(lhs);
        Node dlhs = result;

        dispatch(rhs);
        Node drhs = result;

        result = Builder::add(Builder::mult(dlhs, rhs), Builder::mult(drhs, lhs));
    }

    // TODO
    void pow(NodeType v) override {
        result = zero();
    }

    void neg(NodeType expr) override {
        dispatch(expr);
        result = Builder::neg(result);
    }

    // TODO
    void inv(NodeType) override {
        result = zero();
    }

    void value(NodeType) override {
        result = zero();
    }

    void placeholder(NodeType expr) override {
        Placeholder* p = to_placeholder(expr);

        if (variable == p->name)
            result = one();
        else
            result = zero();
    }

    Node result;
    const std::string& variable;
};

}

#endif
