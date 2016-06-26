

namespace symdiff{

/*
 *  Using the visitor pattern might actually help me implement parallel
 *  evaluation, as each FullEval instance can be executed in a separate thread
 *
 *  FullEval only works for function that returns numbers.
 *  which is currently the only kind of allowed function.
 */
class FullEval: public Visitor
{
  public:

    // const context, variable immutable
    FullEval(const Context& ctx, SymExpr expr):
      ctx(ctx)
    {
      expr->visit(*this);
    }

    static double full_eval(const Context& ctx, SymExpr expr){
      return FullEval(ctx, expr).result;
    }

    void binary_operator(SymExpr lhs, SymExpr rhs, std::function<double(double, double)> op){
      lhs.visit(*this)
      double lhs_val = result;

      rhs.visit(*this);
      double rhs_val = result;

      result = op(lhs, rhs);
    }

    void unary_operator(SymExpr expr, std::function<double(double)> op){
      expr.visit(*this);
      result = op(result);
    }

    void add(SymExpr lhs, SymExpr rhs){
      return binary_operator(lhs, rhs, [](double x, double y){ return x + y});
    }

    void sub(SymExpr lhs, SymExpr rhs){
      return binary_operator(lhs, rhs, [](double x, double y){ return x - y});
    }

    void mult(SymExpr lhs, SymExpr rhs){
      return binary_operator(lhs, rhs, [](double x, double y){ return x * y});
    }

    void div(SymExpr lhs, SymExpr rhs){
      return binary_operator(lhs, rhs, [](double x, double y){ return x / y});
    }

    void neg(SymExpr expr){
      return unary_operator(expr, [](double x){ return - x;});
    }

    void inv(SymExpr expr){
      return unary_operator(expr, [](double x){ return 1.0 / x;});
    }

    void scalar(SymExpr expr){
      Scalar* s = static_cast<Scalar*>(expr.get());
      result = s.value;
    }

    void placeholder(SymExpr expr){
      Placeholder* p = static_cast<Placeholder*>(expr.get());
      result = ctx.at(p->name);
    }

  double result;
  const Context& ctx;
};

}