#ifndef SYMDIFF_INTERNAL_PRETTYPRINT_HEADER_
#define SYMDIFF_INTERNAL_PRETTYPRINT_HEADER_

#include "Nodes.h"

#include <ostream>
#include <vector>
#include <string>

namespace symdiff{


struct PrettyPrint: public Visitor
{
    PrettyPrint(std::ostream& out):
        out(out)
    {}

    static std::ostream& run(std::ostream& out, Node n){
        PrettyPrint(out).dispatch(n);
        return out;
    }

#define SYMDIFF_NODES_DEFINITIONS
    #define DEFINE_UNARY_NODE(__type__, __str__, __repr__)\
        void __str__ (UnaryNode* n) override {  unary_node(n, #__repr__);}

    #define DEFINE_BINARY_NODE(__type__, __str__, __repr__)\
        void __str__ (BinaryNode* n) override {  binary_node(n, #__repr__);}

    #include "../src/Nodes.def"
#undef SYMDIFF_NODES_DEFINITIONS

    // Binary node printing
    virtual void binary_node(BinaryNode* b, const std::string& repr) {

        bool lhs_precedence = !is_leaf(b->lhs) && b->lhs->id < b->id;
        bool rhs_precedence = !is_leaf(b->rhs) && b->rhs->id < b->id;

        if (lhs_precedence) out << "(";
            this->dispatch(b->lhs);
        if (lhs_precedence) out << ")";

            out << " " << repr << " ";

        if (rhs_precedence) out << "(";
            this->dispatch(b->rhs);
        if (rhs_precedence) out << ")";
    }

    // Unary Node printing
    virtual void unary_node(UnaryNode* u, const std::string& repr) {
        out << repr << "(";
            this->dispatch(u->expr);
        out << ")";
    }

    void catch_all(NodeType) override{
        out << "_";
    }

    void value(Value* n) override{
        out << n->value;
    }

    void placeholder(Placeholder* n) override{
        out << n->name;
    }

    void cond(Cond* c) override{
        out << "if (";
            dispatch(c->cond());
        out << ") {";
            dispatch(c->texpr());
        out << "} else {";
            dispatch(c->fexpr());
        out << "}";
    }

    std::ostream& out;
};

struct PolishPrint: public PrettyPrint
{
    PolishPrint(std::ostream& out):
        PrettyPrint(out)
    {}

    static std::ostream& run(std::ostream& out, Node n){
        PolishPrint(out).dispatch(n);
        return out;
    }

    void binary_node(BinaryNode* b, const std::string& repr) override {
        out << "(" << repr << " "; dispatch(b->lhs); out << " "; dispatch(b->rhs); out << ")";
    }

    void unary_node(UnaryNode* u, const std::string& repr) override {
        out << "(" << repr << " "; dispatch(u->expr); out << ")";
    }

    void cond(Cond* c) override{
        out << "(if ";
        dispatch(c->cond()); out << " ";
        dispatch(c->texpr()); out << " ";
        dispatch(c->fexpr()); out << ")";
    }

};

}

#endif
