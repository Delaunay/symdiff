#ifndef REGISTERVM_H
#define REGISTERVM_H
/*
 * Register Based Virtual Machine
 * SBVM are used by Lua VM
 */

#include "Nodes.h"
#include "Context.h"
#include "Builder.h"

#include <sstream>
#include <vector>
#include <iostream>

namespace symdiff{

// RVM Opcodes
enum class ROpCode: std::size_t{
#define SYMDIFF_NODES_DEFINITIONS
    #define DEFINE_UNARY_NODE(__type__, __str__, __repr__)  __str__,
    #define DEFINE_BINARY_NODE(__type__, __str__, __repr__)  __str__,
    #define DEFINE_RVM_NODE(__type__, __str__, __repr__)  __str__,
    #include "Nodes.def"
#undef SYMDIFF_NODES_DEFINITIONS
};

typedef std::vector<double> Registers;

//enum class RegisterID: Registers::size_type{
//    Return      = 0, // Register with the result
//    Load        = 1, // Register in which constant are loaded
//    Move        = 2, // Move value from register r1 to register r2
//    Lookup      = 3, // Lookup value in ctx and load to register
//    End
//};


struct RInstruction{
    RInstruction(double v, std::size_t expr):
        op(ROpCode::load), r1(expr), value(v)
    {}

    RInstruction(const std::string& name, std::size_t expr):
        op(ROpCode::lookup), r1(expr), name(name)
    {}

    RInstruction(ROpCode op, std::size_t expr):
        op(op), r1(expr), r3(expr)
    {}

    RInstruction(ROpCode op, std::size_t r1, std::size_t r2, std::size_t r3 = 0):
        op(op), r1(r1), r2(r2), r3(r3)
    {}


    ROpCode     op;    // operation
    std::size_t r1{0}; // lhs / expr
    std::size_t r2{0}; // rhs
    std::size_t r3{0}; // return register

    std::string name{""};   // fun-name
    double      value{0};   // arg number
};

typedef std::vector<RInstruction> RInstructions;

inline
std::string to_string(ROpCode o){
    switch(o){
    #define SYMDIFF_NODES_DEFINITIONS
        #define DEFINE_UNARY_NODE(__type__, __str__, __repr__)\
            case ROpCode::__str__: return #__str__;
        #define DEFINE_BINARY_NODE(__type__, __str__, __repr__)\
            case ROpCode::__str__: return #__str__;
        #define DEFINE_RVM_NODE(__type__, __str__, __repr__)\
            case ROpCode::__str__: return #__str__;
        #include "Nodes.def"
    #undef SYMDIFF_NODES_DEFINITIONS
    }
    return "ERROR";
}

inline
std::ostream& print(std::ostream& out, const RInstruction& i){
    out << to_string(i.op);

    switch(i.op){
    case ROpCode::lookup: return out << " " << i.name  << " => " << i.r1;
    case ROpCode::move:   return out << " " << i.r1    << " => " << i.r2;
    case ROpCode::load:   return out << " " << i.value << " => " << i.r1;
    case ROpCode::branch: return out << " " << i.r1 << " ? " << i.r2;
    }
    return out << " " << i.r1 << ", " << i.r2 << ", " << i.r3;
}

inline
std::ostream& print(std::ostream& out, const RInstructions& s){
    int k = 0;
    for(const auto& i: s){
        out << k << " ";
        print(out, i) << "\n";
        k += 1;
    }
    return out;
}

/*
 * Generate a vector of instructions from our AST
 */
class RegisterGenerator: public Visitor
{
  public:

    RegisterGenerator(Node expr)
    {
        dispatch(expr);
    }

    static RInstructions run(NodeType expr){
        return RegisterGenerator(expr).result;
    }

#define SYMDIFF_NODES_DEFINITIONS
    #define DEFINE_BINARY_NODE(__type__, __str__, __repr__) \
    void __str__(BinaryNode* b) override{\
        dispatch(b->lhs); \
        pos = pos + 1;\
        dispatch(b->rhs); \
        \
        result.push_back({ROpCode::__str__, pos - 1, pos, pos - 1});\
        pos = pos - 1;\
    }

    #define DEFINE_UNARY_NODE(__type__, __str__, __repr__) \
    void __str__(UnaryNode* u) override{\
        dispatch(u->expr);\
        \
        result.push_back({ROpCode::__str__, pos});\
    }

    #include "Nodes.def"
#undef SYMDIFF_NODES_DEFINITIONS

    void value(Value* v) override {
        result.push_back({v->value, pos});
    }

    void placeholder(Placeholder* p) override {
        result.push_back({p->name, pos});
    }

    // Branch if r1 is true branch r2
    void cond(Cond* c) override{
        // Add cond instructions
        dispatch(c->cond());

        // insert branch instruction
        result.push_back({ROpCode::branch, pos, 0});
        std::size_t rp = result.size();

        // Branch = if true branch
        // so false instructions must be first
        dispatch(c->fexpr());

        // insert branch instruction that jumps everytime
        // dont override previous temp value
        result.push_back({1, pos + 1});
        result.push_back({ROpCode::branch, pos + 1, 0});
        std::size_t rp2 = result.size();

        dispatch(c->texpr());

        // we cant use reference because the vector might have been realocated...
        // size of fexpr() + branch
        result[rp - 1].r2 = rp2 - rp;

        // size of texpr()
        result[rp2 - 1].r2 = result.size() - rp2;
    }

    std::size_t   pos{0};
    RInstructions result;
};
/*
 *  A register is a single block of memory holding a single value
 *  Register VM manipulates a register `r` where `r` represent
 *  the register's index.
 *
 *  Opcode modify registers
 *
 */
class RegisterVM
{
public:
    RegisterVM(const RInstructions& i):
        instructions(i), registers(Registers(100))
    {}

    static double run(const NameContext& ctx, const RInstructions& v){
        return RegisterVM(v).eval(ctx);
    }

    static double run(const NameContext& ctx, Node expr){
        // compile expr
        auto v = RegisterGenerator::run(expr);
        return RegisterVM(v).eval(ctx);
    }

    double eval(const NameContext& ctx){
        // Virtual CPU - instruction dispatcher
        // we increase by +1 because we dont have branching op yet
        for(std::size_t ic = 0; ic < instructions.size(); ++ic){
            RInstruction inst = instructions[ic];

            switch (inst.op){
            // Memory
            case ROpCode::load:{
                reg(inst.r1) = inst.value;
                break;}

            case ROpCode::move:{
                reg(inst.r2) = reg(inst.r1);
                break;}

            case ROpCode::lookup:{
                reg(inst.r1) = to_value(ctx.at(inst.name))->value;
                break;}

            // Arithmetics
            case ROpCode::mult:{
                reg(inst.r3) = reg(inst.r1) * reg(inst.r2);
                break;}

            case ROpCode::add:{
                reg(inst.r3) = reg(inst.r1) + reg(inst.r2);
                break;}

            case ROpCode::pow:{
                reg(inst.r3) = std::pow(reg(inst.r1), reg(inst.r2));
                break;}

            case ROpCode::inv:{
                reg(inst.r3) = 1./reg(inst.r1);
                break;}

            case ROpCode::neg:{
                reg(inst.r3) = - reg(inst.r2);
                break;}

            case ROpCode::branch:{
                if (reg(inst.r1) > 0)
                    ic += inst.r2;
                break;}
            }
        }

        return reg(0);
    }

    void dump(std::ostream& out){
        for(std::size_t i = 0; i < max_pos; i++){
            out << i << ". " << reg(i) << std::endl;
        }
    }

private:
    double& reg(std::size_t i)
    {
        max_pos = std::max(i, max_pos);
        return registers[i];
    }

private:
    const RInstructions& instructions;
    std::size_t          max_pos{0};
    Registers registers;
};


}


#endif
