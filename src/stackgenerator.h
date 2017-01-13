#ifndef STACKGENERATOR_H
#define STACKGENERATOR_H

#include "Nodes.h"
#include "Context.h"
#include "Builder.h"

#include <sstream>
#include <vector>
#include <iostream>

namespace symdiff{


// Last-in First out
template<typename T>
class Stack{
public:
    void push(const T& s) {   stack.push_back(s); }
    void pop()                      {   stack.pop_back();   }

    void print(std::ostream& out) const{
        for(const auto& a: stack)
            out << a << "\n";
    }

private:
    std::vector<T> stack;
};

typedef std::vector<std::string> InstructionStack;

/*
 * Generate a vector/stack of instructions from a tree
 */
class StackGenerator: public Visitor
{
  public:

    StackGenerator(/*const NameContext& ctx,*/ Node expr)/*:
        ctx(ctx)*/
    {
        dispatch(expr);
    }

    static InstructionStack run(/*const NameContext& ctx,*/ NodeType expr){
        return StackGenerator(/*ctx,*/ expr).result;
    }

    void value(NodeType expr) override {
        result.push_back("push " + std::to_string(to_value(expr)->value));
    }

    void add(NodeType expr) override{
        // push leaf first
        BinaryNode* b = to_binary(expr);

        dispatch(b->lhs); // push x
        dispatch(b->rhs); // push y

        result.push_back("add");
    }

    void mult(NodeType expr) override{
        // push leaf first
        BinaryNode* b = to_binary(expr);

        dispatch(b->lhs); // push x
        dispatch(b->rhs); // push y

        result.push_back("mult");
    }

    void pow(NodeType expr) override{
        // push leaf first
        BinaryNode* b = to_binary(expr);

        dispatch(b->lhs); // push x
        dispatch(b->rhs); // push y

        result.push_back("pow");
    }

    void neg(NodeType expr) override{
        // push leaf first
        UnaryNode* u = to_unary(expr);

        dispatch(u->expr);

        result.push_back("neg");
    }

    void inv(NodeType expr) override{
        // push leaf first
        UnaryNode* u = to_unary(expr);

        dispatch(u->expr);

        result.push_back("inv");
    }

    void placeholder(NodeType expr) override {
        Placeholder* p = to_placeholder(expr);
        result.push_back("push " + p->name);
        /*
        if (ctx.count(p->name) == 0)
            result.push_back("push " + p->name);

        else{ // Nodes are immutable
            // nothing day the expr is a value technically
            double d = to_value(ctx.at(p->name))->value;
            result.push_back("push " + std::to_string(d));
        } */
    }

  InstructionStack result;
  //const NameContext& ctx;
};

// Eval a Stack
class StackVM
{
public:
    StackVM(const InstructionStack& v):
        instructions(v)
    {}

    static double run(const NameContext& ctx, const InstructionStack& v){
        return StackVM(v).eval(ctx);
    }

    double eval(const NameContext& ctx){

        for(std::size_t pc = 0; pc < instructions.size(); ++pc){
            // split
            std::stringstream ss(instructions[pc]);
            std::string op, arg;

            ss >> op >> arg;

            if (op == "push"){
                if (arg == ""){ std::cout << "WARNING \n"; }
                // Get value from Context
                double d = to_value(ctx.at(arg))->value;
                temp.push_back(d);
            }
            else if (op == "add"){
                // read the two arguments
                double rhs = temp.back(); temp.pop_back();
                double lhs = temp.back(); temp.pop_back();
                double t = rhs + lhs;
                temp.push_back(t);
            }
            else if (op == "mult"){
                // read the two arguments
                double rhs = temp.back(); temp.pop_back();
                double lhs = temp.back(); temp.pop_back();
                double r = rhs * lhs;
                temp.push_back(r);
            }
            else{
                std::cout << op << " not implemented" << std::endl;
            }
        }

        return temp[0];
    }

private:
    std::vector<double> temp;
    const InstructionStack& instructions;
};

}


#endif // STACKGENERATOR_H
