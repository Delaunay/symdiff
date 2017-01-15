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

    void add(NodeType n) override {
        BinaryNode* b = to_binary(n);
        return binary_operator(b->lhs, b->rhs, Builder::add, Operator::add);
    }

//    void sub(NodeType lhs, NodeType rhs){
//        BinaryNode* b = to_binary(n);
//        return binary_operator(b->lhs, b->rhs, Builder::sub, , Operator::sub);
//    }

    void mult(NodeType n) override {
        BinaryNode* b = to_binary(n);
        return binary_operator(b->lhs, b->rhs, Builder::mult, Operator::mult);
    }

    void pow(NodeType n) override {
        BinaryNode* b = to_binary(n);
        return binary_operator(b->lhs, b->rhs, Builder::pow, Operator::pow);
    }

//    void div(NodeType lhs, NodeType rhs){
//        BinaryNode* b = to_binary(n);
//        return binary_operator(b->lhs, b->rhs, Builder::div, , Operator::div);
//    }

    void neg(NodeType n) override {
        UnaryNode* b = to_unary(n);
        return unary_operator(b->expr, Builder::neg, Operator::neg);
    }

    void inv(NodeType n) override {
        UnaryNode* b = to_unary(n);
        return unary_operator(b->expr, Builder::inv, Operator::inv);
    }

    void value(NodeType expr) override {
        result = expr;
    }

    void placeholder(NodeType expr) override {
        Placeholder* p = to_placeholder(expr);

        if (ctx.count(p->name) == 0)
            result = expr;

        else // Node are immutable
          result = const_cast<NodeType>(ctx.at(p->name));
    }

  Node result;
  const NameContext& ctx;
};



}

#endif
