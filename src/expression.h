#ifndef SYMDIF_EXPR_HEADER
#define SYMDIF_EXPR_HEADER

#include <functional>
#include <memory>        // shared_ptr
#include <ostream>       // output stream
#include <unordered_map> // Hashtable

namespace symdiff{

// Forward declaration
namespace internal{
    class Expression;
    typedef std::shared_ptr<Expression> SymExpr;
}
typedef internal::SymExpr SymExpr;
typedef std::unordered_map<std::string, SymExpr> Context;

class Error{
public:

    Error(const std::string& str):
        _msg(str)
    {}

    const char* what() const noexcept{  return _msg.c_str(); }

private:
    std::string _msg;
};

class FullEvalError: public Error{
public:
    FullEvalError(const std::string& str): Error(str) {}
};

class CastError: public Error{
public:
    CastError(const std::string& str): Error(str) {}
};

namespace internal{

    enum ExprSubType
    {
        // Leafs
        EST_Scalar,
        EST_Placeholder,

        // Nodes
        EST_Add,
        EST_Mult,
    };

    //                          Base Class
    // =========================================================================

    class Expression{
    public:
        virtual ~Expression() {}

        /* Print representation */
        virtual std::ostream& print(std::ostream& out) = 0;

        // Special constructor
        // This is the constructor that should be used internally
        // it makes simplification. It does not always return the
        // type of the constructed object (i.e Add(3, Add(3, 3)) -> Mult(3, 3)
        // that is why it cannot be implemented in the usual constructor
        // T(arg1, arg1) => Expression::make(arg1, arg2)
        // this is a default and generic version of the function
        // that can be used as a base case (no simplification)
        template<typename T, typename... Args>
        static SymExpr make(Args&&... args){
            return std::make_shared<T>(std::forward<Args>(args)...);
        }

        // This is not a mathematical < operator but rather
        // a function that force operation order inside an expression
        // i.e  (2 * a) and (a * 2) are the same and should be represented the same way
        // instead of having to check for both we force
        // Scalar to be first (order is given by ExprSubType ID)
        // i.e if you input (a * 2) symdiff will reorder terms as (2 * a)
        // reordering is only done for associative nodes (duh)

        // Reordering terms is useful for two main reasons:
        //      - Only one term need to be checked during simplification
        //      - The same fringe problem with associative nodes become trivial
        //              (used to identify node equality)
        bool operator < (const Expression& a) {
            return ExprSubType (*this) < ExprSubType (a);
        }

        // used for simplification
        virtual bool is_nul()    {  return false;   }
        virtual bool is_one()    {  return false;   }
        virtual bool is_scalar() {  return false;   }
        virtual bool is_leaf()   {  return false;   }

        // basic graph transformation
        virtual double full_eval(Context& c)  = 0;
        virtual SymExpr partial_eval(Context& c) = 0;
        virtual SymExpr derivate(const std::string& name) = 0;

        // implicit conversion to ExprSubType so we can switch over expression
        virtual operator ExprSubType () const = 0;
    private:
    };

    // reorder is a conditional swap
    void reorder(SymExpr& a, SymExpr& b);

    //                             LEAFS
    // =========================================================================

    // Pre allocate common variables
    SymExpr minus_one();
    SymExpr zero();
    SymExpr one();
    SymExpr two();

    // retrieve a value from a Scalar
    double get_value(SymExpr v);
    const std::string&  get_name(SymExpr v);

    template<typename T>
    class Scalar: public Expression
    {
    public:
        Scalar(T v):
            _value(v)
        {}

        static SymExpr make(T v){
            if (v == 0)
                return zero();
            if (v == 1)
                return one();
            if (v == -1)
                return minus_one();
            if (v == 2)
                return two();

            return Expression::make<Scalar>(v);
        }

        virtual std::ostream& print(std::ostream& out) {    return out << _value; }

        // Helpers
        operator ExprSubType () const { return EST_Scalar; }
        virtual bool is_nul()    {  return _value == 0;   }
        virtual bool is_one()    {  return _value == 1;   }
        virtual bool is_scalar() {  return true;   }
        virtual bool is_leaf()   {  return true;   }

        // Transformation
        SymExpr derivate(const std::string& name){  return zero(); }
        double full_eval(Context& c)             {  return _value;  }
        virtual SymExpr partial_eval(Context& c) {  return Scalar::make(_value); }

        T value() {    return _value; }

    private:
        T _value;
    };

    typedef Scalar<double> ScalarDouble;
    typedef Scalar<int> ScalarInt;

    class Placeholder: public Expression
    {
    public:
        Placeholder(const std::string& name):
            _name(name)
        {}

        static SymExpr make(const std::string& name){
            return Expression::make<Placeholder>(name);
        }

        virtual std::ostream& print(std::ostream& out) {    return out << _name; }

        operator ExprSubType () const  { return EST_Placeholder; }
        virtual bool is_leaf()   {  return true;   }

        SymExpr derivate(const std::string& name) {
            if (name == _name)
                return one();
            return zero();
        }

        double full_eval(Context& c){
            if (c.find(_name) != c.end())
                return c[_name]->full_eval(c);
            throw FullEvalError("Placeholder variable was not set");
        }

        virtual SymExpr partial_eval(Context& c) {
            if (c.find(_name) != c.end())
                return c[_name];
            return Placeholder::make(_name);
        }

        const std::string& name() { return _name;   }

    private:
        std::string _name;
    };

    //                          NODES
    // =========================================================================

    class UnaryOperator: public Expression
    {
    public:
        UnaryOperator(SymExpr expr):
            _expr(expr)
        {}

    protected:
        SymExpr _expr;
    };


    class BinaryOperator: public Expression
    {
    public:
        typedef std::function<double(double, double)> BinaryFunction;

        BinaryOperator(SymExpr lhs, SymExpr rhs):
            _lhs(lhs), _rhs(rhs)
        {}

        std::ostream& print(std::ostream& out, const std::string& op) {
            out << '('; _lhs->print(out) << " " << op << " ";
            return      _rhs->print(out) << ')';
        }

        double full_eval(Context& c, BinaryFunction f){
            return f(_lhs->full_eval(c), _rhs->full_eval(c));
        }

        template<typename Node>
        SymExpr partial_eval(Context& c, BinaryFunction f) {

            SymExpr lhs = _lhs->partial_eval(c);
            SymExpr rhs = _rhs->partial_eval(c);

            // simplify
            if (lhs->is_scalar() && rhs->is_scalar()){
                double r = get_value(rhs);
                double l = get_value(lhs);
                return ScalarDouble::make(f(r, l));
            }

            // carry
            return Node::make(lhs, rhs);
        }

    protected:
        SymExpr _lhs;
        SymExpr _rhs;
    };

    //  ADD
    // -------------------

    class Add: public BinaryOperator
    {
    public:
        Add(SymExpr lhs, SymExpr rhs):
            BinaryOperator(lhs, rhs)
        {}

        static SymExpr make(SymExpr lhs, SymExpr rhs);

        BinaryFunction add_function(){
            return [](double a, double b){ return a + b; };
        }

        std::ostream& print(std::ostream& out) {
            return BinaryOperator::print(out, "+");
        }

        double full_eval(Context& c){
            return BinaryOperator::full_eval(c, add_function());
        }

        SymExpr partial_eval(Context& c) {
            return BinaryOperator::partial_eval<Add>(c, add_function());
        }

        SymExpr derivate(const std::string& name) {
            SymExpr rhs = _rhs->derivate(name);
            SymExpr lhs = _lhs->derivate(name);
            return Add::make(rhs, lhs);
        }

        operator ExprSubType () const  { return EST_Add; }
    };

    //  Mult
    // -------------------
    class Mult: public BinaryOperator
    {
    public:
        Mult(SymExpr lhs, SymExpr rhs):
            BinaryOperator(lhs, rhs)
        {}

        static SymExpr make(SymExpr lhs, SymExpr rhs);

        BinaryFunction mult_function(){
            return [](double a, double b){ return a * b; };
        }

        std::ostream& print(std::ostream& out) {
            return BinaryOperator::print(out, "*");
        }

        double full_eval(Context& c){
            return BinaryOperator::full_eval(c, mult_function());
        }

        SymExpr partial_eval(Context& c) {
            return BinaryOperator::partial_eval<Mult>(c, mult_function());
        }

        SymExpr derivate(const std::string& name) {
            SymExpr rhs = _rhs->derivate(name);
            SymExpr lhs = _lhs->derivate(name);

            auto first = Mult::make(rhs, _lhs);
            auto sec = Mult::make(_rhs, lhs);

            return Add::make(first, sec);
        }

        operator ExprSubType () const  { return EST_Mult; }
    };
}


}

#endif
