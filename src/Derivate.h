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

    void add(BinaryNode* b) override {
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

    void mult(BinaryNode* b) override {
        NodeType rhs = b->rhs;
        NodeType lhs = b->lhs;

        dispatch(lhs);
        Node dlhs = result;

        dispatch(rhs);
        Node drhs = result;

        result = Builder::add(Builder::mult(dlhs, rhs), Builder::mult(drhs, lhs));
    }

    // TODO
    void pow(BinaryNode* b) override {
        result = zero();
    }

    void neg(UnaryNode* u) override {
        dispatch(u->expr);
        result = Builder::neg(result);
    }

    // TODO
    void inv(UnaryNode*) override {
        result = zero();
    }

    void value(Value*) override {
        result = zero();
    }

    void placeholder(Placeholder* p) override {
        if (variable == p->name)
            result = one();
        else
            result = zero();
    }

    void cond(Cond* c) override{
        // derivative not always defined
        dispatch(c->texpr()); Node b = result;
        dispatch(c->fexpr());
        result = make_cond(c->cond(), b, result);
    }

    Node result;
    const std::string& variable;
};

}

#endif
