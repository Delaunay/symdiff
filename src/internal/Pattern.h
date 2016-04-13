#ifndef SYMDIF_INTERNAL_PATTERN_HEADER
#define SYMDIF_INTERNAL_PATTERN_HEADER

/*
 * Implement pattern for easy search & replace inside an expression
 */

#include "AbstractExpression.h"
#include "Errors.h"


/*
 *  class Pattern: public: Expression   // Pattern cannot be evaluated
 *  class UnaryPat: public Pattern
 *  class BinaryPat: public Pattern
 *  class LeafPat: public Pattern
 *  class AnyPat: public Pattern
 */

namespace symdiff{
namespace internal {

class Pattern: public Expression
{
public:


    // basic graph transformation
    double full_eval(Context& c) final              {   throw EvalError("Patterns can not be evaluated"); }
    SymExpr partial_eval(Context& c) final          {   throw EvalError("Patterns can not be evaluated"); }
    double full_eval(PtrContext& c)  final          {   throw EvalError("Patterns can not be evaluated"); }
    SymExpr partial_eval(PtrContext& c) final       {   throw EvalError("Patterns can not be evaluated"); }
    SymExpr derivate(const std::string& name) final {   throw EvalError("Patterns can not be evaluated"); }
    int depth(int i = 0) {  return i;   }

    std::ostream& gen(std::ostream& out, OutputType t) final { return this->print(out); }

    bool equal(SymExpr& s) final {  return sym_equal(s);  }
    bool is_pattern() final      {  return true;          }

    // this might be useful
    SymExpr substitute(Context& x) {}

    //SymExpr& matched_expr() ;
private:
    // SymExpr _matched_expr;

};

class LeafPat: public Pattern
{
public:
    bool sym_equal(SymExpr& s) { return s->is_leaf();   }
    std::ostream& print(std::ostream& out) {    return out << "<leaf>"; }

    bool is_binary() {  return false;    }
    bool is_leaf()   {  return true;     }

    ExprSubType get_type() const {  return LEAF_SUBTYPE; }
};

class UnaryPat: public Pattern
{
public:
    bool sym_equal(SymExpr& s) { return !s->is_binary();   }
    std::ostream& print(std::ostream& out) {    return out << "<ury>"; }

    bool is_binary() {  return false;    }
    bool is_leaf()   {  return false;    }

    ExprSubType get_type() const {  return UNARY_SUBTYPE; }
};

class BinaryPat: public Pattern
{
public:
    bool sym_equal(SymExpr& s) { return s->is_binary();   }
    std::ostream& print(std::ostream& out) {    return out << "<bry>"; }

    bool is_binary() {  return true;    }

    ExprSubType get_type() const {  return BINARY_SUBTYPE; }
};

class AnyPat: public Pattern
{
public:
    bool sym_equal(SymExpr& ) { return true;   }
    std::ostream& print(std::ostream& out) {    return out << "<any>"; }

    bool is_binary() {  return true;    }
    bool is_leaf()   {  return true;    }

    ExprSubType get_type() const {  return PATTERN_SUBTYPE; }

};

// Only a single instance of each pattern type can be created
// Unless matching pattern are populated
SymExpr leaf();
SymExpr unary();
SymExpr binary();
SymExpr any();

}
}






#endif
