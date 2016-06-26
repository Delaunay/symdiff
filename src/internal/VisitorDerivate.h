

namespace symdiff{

class Derivate: public Visitor
{
  public:

    // const context, variable immutable
    Derivate(const Context& ctx, SymExpr expr):
      ctx(ctx)
    {
      expr->visit(*this);
    }

    static SymExpr derivate(const Context& ctx, SymExpr expr){
      return Derivate(ctx, expr).result;
    }

    void add(SymExpr lhs, SymExpr rhs){
      lhs.visit(*this)  SymExpr dlhs = result;
      rhs.visit(*this); SymExpr drhs = result;

      result = Builder::add(dlhs, drhs);
    }

    void sub(SymExpr lhs, SymExpr rhs){
      lhs.visit(*this)  SymExpr dlhs = result;
      rhs.visit(*this); SymExpr drhs = result;

      result = Builder::sub(dlhs, drhs);
    }

    void mult(SymExpr lhs, SymExpr rhs){
      lhs.visit(*this)  SymExpr dlhs = result;
      rhs.visit(*this); SymExpr drhs = result;

      result = Builder::add(Builder::mult(dlhs, rhs), Builder::mult(drhs, lhs));
    }

    void neg(SymExpr expr){
      expr.visit(*this);
      result = Builder::neg(result);
    }

    void scalar(SymExpr expr){
      result = zero();
    }

    void placeholder(SymExpr expr){
      Placeholder* p = static_cast<Placeholder*>(expr.get());
      result = ctx.at(p->name) ? one() : zero();
    }

  SymExpr result;
  const Context& ctx;
};

}