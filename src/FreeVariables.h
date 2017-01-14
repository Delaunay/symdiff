class FreeVariables: public Visitor
{
public:
    FreeVariables(NodeType expr){
        dispatch(expr);
    }

    static std::vector<std::string> run(NodeType expr){
        return FreeVariables(expr).var;
    }

#define SYMDIFF_NODES_DEFINITIONS
    #define DEFINE_BINARY_NODE(__type__, __str__, __repr__) \
    void __str__(NodeType expr) override{\
        BinaryNode* b = to_binary(expr);\
        dispatch(b->lhs);\
        dispatch(b->rhs);\
    }
    #define DEFINE_UNARY_NODE(__type__, __str__, __repr__) \
    void __str__(NodeType expr) override{\
        UnaryNode* u = to_unary(expr);\
        dispatch(u->expr);\
    }
    #include "Nodes.def"
#undef SYMDIFF_NODES_DEFINITIONS

    void value(NodeType) override { return; }

    void placeholder(NodeType expr) override {
        Placeholder* p = to_placeholder(expr);
        var.push_back(p->name);
    }

private:
    std::vector<std::string> var;
};