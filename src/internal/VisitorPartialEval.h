

namespace symdiff{

/*
 *  Using the visitor pattern might actually help me implement parallel
 *  evaluation, as each FullEval instance can be executed in a separate thread
 */
class PartialEval: public Visitor
{
  public:

    // const context, variable immutable
    PartialEval(const Context& ctx, SymExpr expr):
      ctx(ctx)
    {
      expr->visit(*this);
    }

    static SymExpr partial_eval(const Context& ctx, SymExpr expr){
      return PartialEval(ctx, expr).result;
    }

    void binary_operator(SymExpr lhs, SymExpr rhs, std::function<SymExpr(SymExpr, SymExpr)> op){
      lhs.visit(*this)
      SymExpr lhs_val = result;
      // technically I could implement some kind of shortcut mechanism
      // for operation like mult i.e if (0 * rhs) do not visit rhs
      rhs.visit(*this);
      SymExpr rhs_val = result;

      result = op(lhs, rhs);
    }

    void unary_operator(SymExpr expr, std::function<SymExpr(SymExpr)> op){
      expr.visit(*this);
      result = op(result);
    }

    void add(SymExpr lhs, SymExpr rhs){
      return binary_operator(lhs, rhs, Builder::add);
    }

    void sub(SymExpr lhs, SymExpr rhs){
      return binary_operator(lhs, rhs, Builder::sub);
    }

    void mult(SymExpr lhs, SymExpr rhs){
      return binary_operator(lhs, rhs, Builder::mult);
    }

    void div(SymExpr lhs, SymExpr rhs){
      return binary_operator(lhs, rhs, Builder::div);
    }

    void neg(SymExpr expr){
      return unary_operator(expr, Builder::neg);
    }

    void inv(SymExpr expr){
      return unary_operator(expr, Builder::inv);
    }

    void scalar(SymExpr expr){
      result = expr;
    }

    void placeholder(SymExpr expr){
      Placeholder* p = static_cast<Placeholder*>(expr.get());
      SymExpr v = ctx.at(p->name);

      if (v)
        result = v;
      else
        result = zero();
    }

  SymExpr result;
  const Context& ctx;
};

}