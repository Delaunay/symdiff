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

    void add(NodeType n) override {
        BinaryNode* b = to_binary(n);
        return binary_operator(b->lhs, b->rhs, Builder::add);
    }

//    void sub(NodeType lhs, NodeType rhs) override {
//        BinaryNode* b = to_binary(n);
//        return binary_operator(b->lhs, b->rhs, Builder::sub, , Operator::sub);
//    }

    void mult(NodeType n) override {
        BinaryNode* b = to_binary(n);
        return binary_operator(b->lhs, b->rhs, Builder::mult);
    }

    void pow(NodeType n) override {
        BinaryNode* b = to_binary(n);
        return binary_operator(b->lhs, b->rhs, Builder::pow);
    }

//    void div(NodeType lhs, NodeType rhs) override {
//        BinaryNode* b = to_binary(n);
//        return binary_operator(b->lhs, b->rhs, Builder::div, , Operator::div);
//    }

    void neg(NodeType n) override {
        UnaryNode* b = to_unary(n);
        return unary_operator(b->expr, Builder::neg);
    }

    void inv(NodeType n) override {
        UnaryNode* b = to_unary(n);
        return unary_operator(b->expr, Builder::inv);
    }

    void value(NodeType expr) override {
        Value* s = to_value(expr);
        result = Builder::value(s->value);
    }

    void placeholder(NodeType expr) override {
        Placeholder* p = to_placeholder(expr);
        result = Builder::placeholder(p->name);
    }

    Node result;
};

}

#endif
