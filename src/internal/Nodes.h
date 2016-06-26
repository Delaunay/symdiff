#ifndef SYMDIFF_INTERNAL_NODES
#define SYMDIFF_INTERNAL_NODES

#include <memory>

namespace symdiff{

// RTTI
enum class TypeID : std::size_t{
    #define SYMDIFF_NODES_DEFINITIONS
        #define DEFINE_LEAF_NODE(__type__, __str__, __repr__)  __str__,
        #define DEFINE_UNARY_NODE(__type__, __str__, __repr__)  __str__,
        #define DEFINE_BINARY_NODE(__type__, __str__, __repr__)  __str__,
        #include "Nodes.def"
        Size
    #undef SYMDIFF_NODES_DEFINITIONS
};

namespace internal{

    struct Visitor
    {
        virtual void dispatch(class NodeImpl* ) = 0;
    };

    struct NodeImpl{
        NodeImpl(TypeID id):
            type(id)
        {}

        // RTTI
        TypeID type;

        // Visitor Pattern
        void _visit(Visitor& v) { v.dispatch(this); }
  };
}

typedef std::shared_ptr<internal::NodeImpl> Node;

struct Leaf: public internal::NodeImpl{
    Leaf(TypeID _id):
      internal::NodeImpl(_id)
    {}
};

struct UnaryNode: public internal::NodeImpl{
    UnaryNode(TypeID _id, Node& _expr):
        internal::NodeImpl(_id), expr(_expr)
    {}

    Node expr;
};

struct BinaryNode: public internal::NodeImpl{
    BinaryNode(TypeID _id, Node& _lhs, Node& _rhs):
        internal::NodeImpl(_id), lhs(_lhs), rhs(_rhs)
    {}

    Node lhs;
    Node rhs;
};

// Leaf are special nodes
struct Scalar: public internal::NodeImpl
{
    Scalar(TypeID _id, double _value):
        internal::NodeImpl(_id), value(_value)
    {}

    double value;
};

struct Placeholder: public internal::NodeImpl
{
    Placeholder(TypeID _id, const std::string& _name):
        internal::NodeImpl(_id), name(_name)
    {}

    std::string name;
};

inline Node make_scalar(double val){
    return std::make_shared<Scalar>(TypeID::scalar, val);
}

inline Node make_placeholder(const std::string& name){
    return std::make_shared<Placeholder>(TypeID::placeholder, name);
}

// Generate functions
#define SYMDIFF_NODES_DEFINITIONS
    //  create make_node_name() function with correct RTTI
    #define DEFINE_UNARY_NODE(__type__, __str__, __repr__) \
        inline Node make_##__str__(Node& val){\
            return std::make_shared<UnaryNode>(TypeID::__str__, val);\
        }

    #define DEFINE_BINARY_NODE(__type__, __str__, __repr__) \
        inline Node make_##__str__(Node& lhs, Node& rhs){\
            return std::make_shared<BinaryNode>(TypeID::__str__, lhs, rhs);\
        }

    #include "Nodes.def"
#undef SYMDIFF_NODES_DEFINITIONS



}

#endif
