#ifndef SYMDIF_EXPR_HEADER
#define SYMDIF_EXPR_HEADER

#include <memory>        // shared_ptr
#include <ostream>       // output stream
#include <unordered_map> // Hashtable

namespace symdif{

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
    class Expression{
    public:
        virtual ~Expression() {}

        /* Print representation */
        virtual std::ostream& print(std::ostream& out) = 0;

        virtual bool is_nul()    {  return false;   }
        virtual bool is_one()    {  return false;   }
        virtual bool is_scalar() {  return false;   }
        virtual bool is_leaf()   {  return false;   }

        virtual double full_eval(Context& c)  = 0;
        virtual SymExpr partial_eval(Context& c) = 0;
        virtual SymExpr derivate(const std::string& name) = 0;
    private:
    };

    SymExpr one();
    SymExpr zero();

    class ScalarDouble: public Expression
    {
    public:
        ScalarDouble(double v):
            _value(v)
        {}

        virtual std::ostream& print(std::ostream& out) {    return out << _value; }

        virtual bool is_nul()    {  return _value == 0;   }
        virtual bool is_one()    {  return _value == 1;   }
        virtual bool is_scalar() {  return true;   }
        virtual bool is_leaf()   {  return true;   }
        SymExpr derivate(const std::string& name) { return zero(); }

        double full_eval(Context& c)             {    return _value;  }
        virtual SymExpr partial_eval(Context& c) {    return SymExpr(new ScalarDouble(_value)); }

        double value() {    return _value; }

    private:
        double _value;
    };

    double get_value(SymExpr v);

    class Placeholder: public Expression
    {
    public:
        Placeholder(const std::string& name):
            _name(name)
        {}

        virtual std::ostream& print(std::ostream& out) {    return out << _name; }

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
            return SymExpr(new Placeholder(_name));
        }

    private:
        std::string _name;
    };


    class Add: public Expression
    {
    public:
        Add(SymExpr lhs, SymExpr rhs):
            _lhs(lhs), _rhs(rhs)
        {}

        virtual std::ostream& print(std::ostream& out) {
            out << '('; _lhs->print(out) << " + ";
            return      _rhs->print(out) << ')';
        }

        double full_eval(Context& c){ return _lhs->full_eval(c) + _rhs->full_eval(c); }

        virtual SymExpr partial_eval(Context& c) {

            SymExpr lhs = _lhs->partial_eval(c);
            SymExpr rhs = _rhs->partial_eval(c);

            // simplify
            if (lhs->is_scalar() && rhs->is_scalar()){
                double r = get_value(rhs);
                double l = get_value(lhs);
                return SymExpr(new ScalarDouble(r + l));
            }

            // carry
            return SymExpr(new Add(lhs, rhs));
        }

        SymExpr derivate(const std::string& name) {

            SymExpr rhs = _rhs->derivate(name);
            SymExpr lhs = _lhs->derivate(name);

            return SymExpr(new Add(rhs, lhs));
        }

    private:
        SymExpr _lhs;
        SymExpr _rhs;
    };

    class Mult: public Expression
    {
    public:
        Mult(SymExpr lhs, SymExpr rhs):
            _lhs(lhs), _rhs(rhs)
        {}

        virtual std::ostream& print(std::ostream& out) {
            out << '('; _lhs->print(out) << " * ";
            return      _rhs->print(out) << ')';
        }

        double full_eval(Context& c){ return _lhs->full_eval(c) * _rhs->full_eval(c); }

        virtual SymExpr partial_eval(Context& c) {

            SymExpr lhs = _lhs->partial_eval(c);
            SymExpr rhs = _rhs->partial_eval(c);

            // simplify
            if (lhs->is_scalar() && rhs->is_scalar()){
                double r = get_value(rhs);
                double l = get_value(lhs);
                return SymExpr(new ScalarDouble(r * l));
            }

            // carry
            return SymExpr(new Mult(lhs, rhs));
        }

        SymExpr derivate(const std::string& name) {

            SymExpr rhs = _rhs->derivate(name);
            SymExpr lhs = _lhs->derivate(name);

            auto first = SymExpr(new Mult(rhs, _lhs));
            auto sec = SymExpr(new Mult(_rhs, lhs));

            return SymExpr(new Add(first, sec));
        }

    private:
        SymExpr _lhs;
        SymExpr _rhs;
    };
}


}

#endif
