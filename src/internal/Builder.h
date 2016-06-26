

namespace symdiff
{

// Hollow class for now
// might hold architecture info
class Builder{
  public:

  // Helper build nodes
  // Those implement basic optimization (constant folding and trivial simplification)
  // You probably always want to use them.

  // Binary
  static SymExpr add (SymExpr lhs, SymExpr rhs){}
  static SymExpr mult(SymExpr lhs, SymExpr rhs){}
  static SymExpr div (SymExpr lhs, SymExpr rhs){}
  static SymExpr sub (SymExpr lhs, SymExpr rhs){}

  // Unary
  static SymExpr inv(SymExpr expr){}
  static SymExpr neg(SymExpr expr){}

  // Leafs
  // Avoid allocating memory for commonly used value (0, 1, etc...)
  static SymExpr scalar(double val){}

  // This one does not anything special but it is nice to have it for
  // consistency
  static SymExpr placeholder(const std::string& name){}
};


}