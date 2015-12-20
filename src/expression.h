/*
 *  Date: Dec 14 2015
 *
 *  Author: Pierre Delaunay
 *
 *  File: Define Basic AST
 *          - What is an Expression     Expression
 *          - What is a Binary Operator BinaryOp
 *          - What is a Unary Operator  UnaryOp
 *          - What is a Constant        Scalar(Value)   (Most used Object) specialy during Partial Evals
 *          - What is a Variable        Variable(name)
 *
 *  TODO: Replace naked pointers with std::unique_ptr<> for public API function
 *      - Would it be uselful to specify type (Real, integer and all)
 *      - What about Matrices ?
 */
#ifndef SYM_EXPRESSION_H
#define SYM_EXPRESSION_H

#include <unordered_map>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <memory>

namespace sym{

class Scalar;
class BinOp;
class Variable;
class Expression;
class TreeBuilder;

typedef std::shared_ptr<Expression> Expr;
typedef double Real;


// Make Variable Hashable
struct variable_hash{
    std::size_t operator() (const Variable& v) const noexcept;
    std::hash<std::string> _h;
};

typedef std::unordered_map<Variable, Real, variable_hash> Variables;


// reduce memory usage and dynamic allocation
// needed in this file (those two don't need to be freed)
// which currently is problematic
Expr zero();
Expr one();
Expr minus_one();

// Simplify levels
// level1 evaluate finite expr  (no precision loss)
// level2 evaluate all scalar expr (possible precision loss)
enum Simplify
{
    level1,
    level2,
};

enum ExprType{
    tScalar = 0,
    tVariable = 1,
    tMult = 2,
    tAdd = 3,
    tInv = 4,
    tLn = 5,
    tExp = 6,
    tPow = 7
};

/*
 *  Shared Function signature
 *      Make it easier to change them later on
 */
#define DERIVATE Expr derivate(TreeBuilder& tb, Variable& x)
#define EVAL Expr eval(TreeBuilder& tb, Variables& x)
#define TYPE ExprType type()
#define FULL_EVAL Real full_eval(Variables& x)
#define PRINT std::ostream& print(std::ostream& out)
#define SIMPLIFY Expr simplify()
#define COPY Expr copy(TreeBuilder& tb)

#define TB(x) tb.add_node(x)
//
//  Pure Abstract
//
class Expression
{
public:
    virtual ~Expression() = default;

    // derivate in respect to the variable x
    virtual DERIVATE = 0;

    // quick helper
    Expr derivate(TreeBuilder& tb, Expr y){
        Variable& x = (Variable&) *y;
        return derivate(tb, x);
    }

    // return the node type
    virtual TYPE = 0;

    // evaluate the expression using defined Variables
    // it returns an Expr* because it may be a partial eval
    // you can check for a partial eval using is_scalar()
    virtual EVAL = 0;

    // fully evaluate the expression
    // is faster than partial eval since we can collapse the graphs
    // and not use expression
    virtual FULL_EVAL = 0;

    // print a representation of the Expression
    virtual PRINT = 0;

    // needed for factorization
    // check if two Tree are equivalent
    // /!\ too lazy to do the exact thing
    virtual bool is_equal(Expr expr){

        std::stringstream ss1;
        expr->print(ss1);

        std::stringstream ss2;
        this->print(ss2);

        // lol
        return ss1.str() == ss2.str();
    }

    // help simplifying Expression
    virtual bool is_scalar() {  return false;   }
    virtual bool is_nul()    {  return false;   }
    virtual bool is_one()    {  return false;   }

    // make trivial simplification (no dynamic allocation)
    virtual SIMPLIFY {  return Expr(this);    }

    // is the expr pre-allocated
    static bool to_be_deleted(Expr expr){
        return !(expr == zero() || expr == one() || expr == minus_one());
    }

    static bool delete_expr(Expr expr){
        /*
        if (to_be_deleted(expr)){
            delete expr;
            expr = nullptr;
            return true;
        }//*/

        return false;
    }

     virtual COPY = 0;
};

//
//  Keep track of dyn allocation
//
class TreeBuilder
{
public:
//    struct Element{
//        Element(Expr* el, bool own=true):
//            el(el), own(own)
//        {}

//        Expr* el;
//        bool own;
//    };

    TreeBuilder(Expr e):
        _root(e)
    {}

    template<typename T>
    TreeBuilder(Expr l, Expr r):
        _root(new T(l, r))
    {
        _gc.push_back(r);
        _gc.push_back(l);
    }

    Expr add_node(Expr e/*, bool b=true*/){
        _gc.push_back(_root);
        _root = e;
        return e;
    }

    TreeBuilder& operator+ (TreeBuilder& b);
    TreeBuilder& operator* (TreeBuilder& b);

    TreeBuilder operator+ (const TreeBuilder& b) const;
    TreeBuilder operator* (const TreeBuilder& b) const;

//    TreeBuilder& operator= (TreeBuilder& b){
//    }

    ~TreeBuilder(){
//        //if (_root.own)
//            Expression::delete_expr(_root);

//        for(auto& i: _gc)
//            //if (i.own)
//                Expression::delete_expr(i);
    }

    TreeBuilder derivate(const TreeBuilder& x){
        TreeBuilder dx(zero());
        _root->derivate(dx, x._root);
        return dx;
    }

    Expr& root() {  return _root;   }

    void print(){
        _root->print(std::cout);
    }

private:

    Expr _root;
    std::vector<Expr> _gc;
};


const TreeBuilder vvar(const std::string& name);
TreeBuilder function();


class BinOp: public Expression
{
public:

    BinOp(Expr l, Expr r):
        left(l), right(r)
    {}

    ~BinOp(){
        //delete_expr(left);
        //delete_expr(right);
    }

//    template<typename function>
//    Real _full_eval(Variables& x);

//    template<typename T, typename function>
//    Expr* _eval(Variables& v);

    Expr left;
    Expr right;
};

class UnaryOp: public Expression
{
public:

    UnaryOp(Expr e):
        expr(e)
    {}

    ~UnaryOp(){
        //delete_expr(expr);
    }

//    template<typename T, typename function>
//    Expr* _eval(Variables& v);

//    template<typename function>
//    Real _full_eval(Variables& x);

    Expr expr;
};

//
//  Constant
//      Does not make dynamic alloc
class Scalar: public Expression
{
public:
    Scalar(Real val = 0):
        _value(val)
    {}

    DERIVATE override{
        return zero(); // new Scalar(0);
    }

    EVAL override{
        return Expr(this);
    }

    FULL_EVAL override{
        return _value;
    }

    PRINT override{
        out << _value;
        return out;
    }

    TYPE override {  return tScalar;}

    bool is_scalar() override{  return true;          }
    bool is_nul()    override{  return _value == 0;   }
    bool is_one()    override{  return _value == 1;   }

    Real value() const   {   return _value;  }

    COPY override{
        Expr e = Expr(new Scalar(value()));
        return tb.add_node(e);
    }

private:
    Real _value;
};


//
//  Placeholder Variable
//      Does not make dynamic alloc
class Variable: public Expression
{
public:
    // Make Variable Hashable
    Variable(const std::string& name):
        _name(name)
    {}

    bool operator== (const Variable& b) const{
        return _name == b._name;
    }

    DERIVATE override {
        return tb.add_node(this == &x ? one() : zero()); // new Scalar(1): new Scalar(0);
    }

    EVAL override;

    FULL_EVAL override;

    PRINT override{
        out << _name;
        return out;
    }

    const std::string& name() const {   return _name;   }

    TYPE override {  return tVariable;}

    // Variables are kept unique
    COPY override{
        return Expr(this);
    }

private:
    std::string _name;
};


//
// BinOp
//
/*
template<typename T, typename function>
Expr* BinOp::_eval(Variables& v){
    Expr* el = left->eval(v);
    Expr* er = right->eval(v);
    Real vl = 0;
    Real vr = 0;

    if (el->is_scalar()){
       vl = dynamic_cast<Scalar*>(el)->value();

       if (er->is_scalar()){
           vr = dynamic_cast<Scalar*>(er)->value();
           return new Scalar(function(vr, vl));
       }

       return new T(new Scalar(vl), er);
    }

    if (er->is_scalar()){
        vr = dynamic_cast<Scalar*>(er)->value();
        return new T(el, new Scalar(vr));
    }

    return new T(el, er);
}

template<typename function>
Real BinOp::_full_eval(Variables& x) {
    return function(left->full_eval(x), right->full_eval(x));
}


template<typename T, typename function>
Expr* UnaryOp::_eval(Variables& v){

    Expr* e = expr->eval(v);

    if (e->is_scalar()){
        Real v = dynamic_cast<Scalar*>(e)->value();

        return new Scalar(function(v));
    }

    return new T(e);
}

template<typename function>
Real UnaryOp::_full_eval(Variables& x) {
    return function(expr->full_eval(x));
}*/
}



#endif // EXPRESSION_H

