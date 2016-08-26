#ifndef SYMDIFF_INTERNAL_SUBSTITUTION_HEADER_
#define SYMDIFF_INTERNAL_SUBSTITUTION_HEADER_

#include "DeepCopy.h"

namespace symdiff{

/*
 * Substitute placeholder variables to values
 */
class Substitution: public DeepCopy
{
  public:

    Substitution(const NameContext& ctx, Node expr):
      result(zero()), ctx(ctx)
    {
        dispatch(expr);
    }

    static Node run(const NameContext& ctx, NodeType expr){
        return Substitution(ctx, expr).result;
    }

    void value(NodeType expr) override {
        result = expr;
    }

    void placeholder(NodeType expr) override {
        Placeholder* p = to_placeholder(expr);

        if (ctx.count(p->name) == 0)
            result = expr;

        else // Nodes are immutable
          result = const_cast<NodeType>(ctx.at(p->name));
    }

  Node result;
  const NameContext& ctx;
};

}

#endif
