#ifndef SYMDIFF_INTERNAL_LLVM_HEADER_
#define SYMDIFF_INTERNAL_LLVM_HEADER_

#ifdef SYMDIFF_LLVM

#include "Builder.h"
#include "Nodes.h"
#include "Context.h"

// LLVM HEADER


namespace symdiff{

/*
 * Generate LLVM-IR from Symbolic representation
 */
class LLVM: public Visitor
{
  public:

    LLVM(const NameContext& ctx, Node expr):
      result(zero()), ctx(ctx)
    {
        dispatch(expr);
    }

    static Node run(const NameContext& ctx, NodeType expr){
        return LLVM(ctx, expr).result;
    }

  Node result;
  const NameContext& ctx;
};

}

#endif

#endif
