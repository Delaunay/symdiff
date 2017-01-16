#ifndef STACKVM_H
#define STACKVM_H
/*
 *  Stack Based Virtual Machine
 *  SBVM are used by Java Virtual Machine and Cython
 */

#include "Nodes.h"
#include "Context.h"
#include "Builder.h"

#include <vector>
#include <ostream>

namespace symdiff{

// VM Opcodes
enum class SOpCode: std::size_t{
#define SYMDIFF_NODES_DEFINITIONS
    #define DEFINE_UNARY_NODE(__type__, __str__, __repr__)  __str__,
    #define DEFINE_BINARY_NODE(__type__, __str__, __repr__)  __str__,
    #define DEFINE_SVM_NODE(__type__, __str__, __repr__)  __str__,
    #include "Nodes.def"
#undef SYMDIFF_NODES_DEFINITIONS
};

struct SInstruction{
    SInstruction(SOpCode op):
        op(op), name("")
    {}

    SInstruction(double v):
        op(SOpCode::push), value(v)
    {}

    SInstruction(std::string name):
        op(SOpCode::lookup), name(name)
    {}

    SOpCode     op;         // call
    std::string name{""};   // fun-name
    double      value{0};   // arg number
};

typedef std::vector<SInstruction> SInstructions;

inline
std::string to_string(SOpCode o){
    switch(o){
    #define SYMDIFF_NODES_DEFINITIONS
        #define DEFINE_UNARY_NODE(__type__, __str__, __repr__)\
            case SOpCode::__str__: return #__str__;
        #define DEFINE_BINARY_NODE(__type__, __str__, __repr__)\
            case SOpCode::__str__: return #__str__;
        #define DEFINE_SVM_NODE(__type__, __str__, __repr__)\
            case SOpCode::__str__: return #__str__;
        #include "Nodes.def"
    #undef SYMDIFF_NODES_DEFINITIONS
    }
    return "ERROR";
}

inline
std::ostream& print(std::ostream& out, const SInstruction& i){
    out << to_string(i.op);

    switch(i.op){
    case SOpCode::lookup: return out << " " << i.name;
    case SOpCode::push: return out << " " << i.value;
    }
    return out;
}

inline
std::ostream& print(std::ostream& out, const SInstructions& s){
    for(const auto& i: s){
        print(out, i) << "\n";
    }
    return out;
}

/*
 * Generate a vector of instructions from our AST
 */
class StackGenerator: public Visitor
{
  public:

    StackGenerator(Node expr)
    {
        dispatch(expr);
    }

    static SInstructions run(NodeType expr){
        return StackGenerator(expr).result;
    }

#define SYMDIFF_NODES_DEFINITIONS
    #define DEFINE_BINARY_NODE(__type__, __str__, __repr__) \
    void __str__(NodeType expr) override{\
        BinaryNode* b = to_binary(expr);\
        \
        dispatch(b->lhs); \
        dispatch(b->rhs); \
        \
        result.push_back(SOpCode::__str__);\
    }
    #define DEFINE_UNARY_NODE(__type__, __str__, __repr__) \
    void __str__(NodeType expr) override{\
        UnaryNode* u = to_unary(expr);\
        \
        dispatch(u->expr);\
        \
        result.push_back(SOpCode::__str__);\
    }

    #include "Nodes.def"
#undef SYMDIFF_NODES_DEFINITIONS

    void value(NodeType expr) override {
        result.push_back(to_value(expr)->value);
    }

    void placeholder(NodeType expr) override {
        Placeholder* p = to_placeholder(expr);
        result.push_back(p->name);
    }

    SInstructions result;
};

/* Eval a vector of instructions using a stack */
class StackVM
{
public:
    typedef Node& NodeType;

    StackVM(const SInstructions& v):
        instructions(v)
    {}

    static double run(const NameContext& ctx, const SInstructions& v){
        return StackVM(v).eval(ctx);
    }

    static double run(const NameContext& ctx, NodeType expr){
        // compile expr
        auto v = StackGenerator::run(expr);
        return StackVM(v).eval(ctx);
    }

    static double pop(std::vector<double>& temp){
        double p = temp.back(); temp.pop_back();
        return p;
    }

    double eval(const NameContext& ctx){
        // Virtual CPU - instruction dispatcher
        for(std::size_t ic = 0; ic < instructions.size(); ++ic){
            SInstruction inst = instructions[ic];

            switch (inst.op){
            // Memory
            case SOpCode::lookup:{
                double d = to_value(ctx.at(inst.name))->value;
                temp.push_back(d);
                break;}

            case SOpCode::push:{
                temp.push_back(inst.value);
                break;}

            // Arithmetics
            case SOpCode::mult:{
                double rhs = pop(temp), lhs = pop(temp);
                double r = rhs * lhs;
                temp.push_back(r);
                break;}

            case SOpCode::add:{
                double rhs = pop(temp), lhs = pop(temp);
                double r = rhs + lhs;
                temp.push_back(r);
                break;}

            case SOpCode::pow:{
                double rhs = pop(temp), lhs = pop(temp);
                double r = std::pow(rhs, lhs);
                temp.push_back(r);
                break;}

            case SOpCode::inv:{
                temp.push_back(1/pop(temp));
                break;}

            case SOpCode::neg:{
                temp.push_back(- pop(temp));
                break;}
            }
        }

        return temp[0];
    }

private:
    // this is our stack of temporary variables
    std::vector<double> temp;
    const SInstructions& instructions;
};

}


#endif // STACKGENERATOR_H
