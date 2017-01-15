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
        void __str__ (NodeType n) override {  unary_node(n, #__repr__);}

    #define DEFINE_BINARY_NODE(__type__, __str__, __repr__)\
        void __str__ (NodeType n) override {  binary_node(n, #__repr__);}

    #include "../src/Nodes.def"
#undef SYMDIFF_NODES_DEFINITIONS

    // Binary node printing
    void binary_node(NodeType n, const std::string& repr) {
        BinaryNode* b = to_binary(n);

        bool lhs_precedence = !is_leaf(b->lhs) && b->lhs->id < n->id;
        bool rhs_precedence = !is_leaf(b->rhs) && b->rhs->id < n->id;

        if (lhs_precedence) out << "(";
            this->dispatch(b->lhs);
        if (lhs_precedence) out << ")";

            out << " " << repr << " ";

        if (rhs_precedence) out << "(";
            this->dispatch(b->rhs);
        if (rhs_precedence) out << ")";
    }

    // Unary Node printing
    void unary_node(NodeType n, const std::string& repr) {
        UnaryNode* u = to_unary(n);
        out << repr << "(";
            this->dispatch(u->expr);
        out << ")";
    }

    void value(NodeType n) override{
        Value* s = to_value(n);
        out << s->value;
    }

    void placeholder(NodeType n) override{
        Placeholder* s = to_placeholder(n);
        out << s->name;
    }

    std::ostream& out;
};

}

#endif
