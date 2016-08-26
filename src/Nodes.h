#ifndef SYMDIFF_INTERNAL_NODES_HEADER_
#define SYMDIFF_INTERNAL_NODES_HEADER_

#include <memory>
#include <string>
#include <cassert>

#include <vector>
#include <algorithm>

/*
 * This is the library's core. All nodes are defined there.
 * Through the X-Macro trick using Nodes.def
 * I am not sure how wise this decision was yet.
 */

namespace symdiff{

// RTTI
enum class NodeID: std::size_t{
    #define SYMDIFF_NODES_DEFINITIONS
        #define DEFINE_LEAF_NODE(__type__, __str__, __repr__)  __str__,
        #define DEFINE_UNARY_NODE(__type__, __str__, __repr__)  __str__,
        #define DEFINE_BINARY_NODE(__type__, __str__, __repr__)  __str__,
        #include "Nodes.def"
        Size
    #undef SYMDIFF_NODES_DEFINITIONS
};

// Type info
enum class TypeID: std::size_t{
    Integer,
    Float,
    Tensor,
    Boolean,
    UserDefined,
    Size
};


/*
 *  Library users will manipulate an API Wrapper and will never have to hear about
 *  pointers
 *
 *  Why Shared Pointer ? A lot of Nodes are reused when computing derivatives
 *  I think shared pointer allow us to reuse current node. Moreover, I need to
 *  look into smart evaluator that would compute nodes only once and cache the results.
 *
 *  A lot of libraries explicitly create a edge class. I am not sure why.
 *  it is necesary for Tensor diff ?
 *
 */
namespace internal{

    struct NodeImpl{
        NodeImpl(NodeID id):
            id(id)
        {}

        // RTTI
        NodeID id;
    };

}

typedef internal::NodeImpl* NodeRef;
typedef std::shared_ptr<internal::NodeImpl> Node;

bool node_compare_le(const Node& a, const Node& b);

/*
 * The Visitor is selfcontained. Both indirection used to implement it
 * are contained in the base class which means tree nodes do not have
 * vtables.
 */
struct Visitor
{
    // I am not sure yet how the API should feel
    typedef Node& NodeType;

    virtual ~Visitor() {}

    void dispatch(NodeType n)  {
        switch(n->id){
            // I do think the compiler will make it O(1)
            #define SYMDIFF_NODES_DEFINITIONS
                #define DEFINE_LEAF_NODE(__type__, __str__, __repr__)  \
                        case NodeID::__str__: \
                            __str__(n); break;\

                #define DEFINE_UNARY_NODE(__type__, __str__, __repr__) \
                        case NodeID::__str__: \
                            __str__(n); break;\

                #define DEFINE_BINARY_NODE(__type__, __str__, __repr__)\
                        case NodeID::__str__: \
                            __str__(n); break;\

                #include "../src/Nodes.def"
            #undef SYMDIFF_NODES_DEFINITIONS
            case NodeID::Size:{
                assert(true && "unreachable");
            }
        }
    }

    #define SYMDIFF_NODES_DEFINITIONS
        #define DEFINE_LEAF_NODE(__type__, __str__, __repr__)   virtual void __str__ (NodeType n) = 0;
        #define DEFINE_UNARY_NODE(__type__, __str__, __repr__)  virtual void __str__ (NodeType n) = 0;
        #define DEFINE_BINARY_NODE(__type__, __str__, __repr__) virtual void __str__ (NodeType n) = 0;

        #include "../src/Nodes.def"
    #undef SYMDIFF_NODES_DEFINITIONS
};

/*
 *  Nodes def
 */
struct Leaf: public internal::NodeImpl{
    Leaf(NodeID _id):
      internal::NodeImpl(_id)
    {}
};

struct UnaryNode: public internal::NodeImpl{
    UnaryNode(NodeID _id, Node& _expr):
        internal::NodeImpl(_id), expr(_expr)
    {}

    Node expr;
};

struct BinaryNode: public internal::NodeImpl{
    BinaryNode(NodeID _id, Node& _lhs, Node& _rhs):
        internal::NodeImpl(_id), lhs(_lhs), rhs(_rhs)
    {}

    Node lhs;
    Node rhs;
};

/*
 * NaryNodes are made for associative operations (+ / *)
 * children are sorted so equality checking is easier
 */
struct NaryNode: public internal::NodeImpl{
public:
    // Minimum 2?
    NaryNode(NodeID _id, Node& _lhs, Node& _rhs):
        internal::NodeImpl(_id), nodes({_lhs, _rhs})
    {
        std::sort(nodes.begin(), nodes.end(), node_compare_le);
    }

    void add_child(Node& n)
    {
        auto it = std::lower_bound(nodes.begin(), nodes.end(), n, node_compare_le);
        nodes.insert(it, n);
    }

    Node operator[] (std::size_t idx) const {  return nodes[idx]; }
    std::size_t size() const {  return nodes.size(); }

private:
    std::vector<Node> nodes;
};



// Leaf are specials nodes
struct Value: public internal::NodeImpl
{
    Value(double _value):
        internal::NodeImpl(NodeID::value), value(_value), type(TypeID::Float)
    {}

    // FIXME: use boost.variant
    // CHECK: std::variant in C++17

//    struct Tensor{
//        void* data; // Because Eigen::Map FTW
//        std::vector<std::size_t> size;
//        TypeID type;
//    };

//    union ValueImpl {
//         APint   apint;   // (AP) Arbitrary Precision
//         APfloat apfloat;
//         Tensor  tensor;
//         int32   i32;
//         float32 f32;
//         int32   i64;
//         float32 f64;
//         int16   i16;
//         float16 f16;
//         int8    i8;
//    };

//     TypeID type;
//     ValueImpl value;
    double value;
    TypeID type;
};

struct Placeholder: public internal::NodeImpl
{
    Placeholder(const std::string& _name):
        internal::NodeImpl(NodeID::placeholder), name(_name)
    {}

    std::string name;
};

// Sort Expressions
inline
bool node_compare_le(const Node& a, const Node& b){
    NodeID ai = a->id;
    NodeID bi = b->id;

    if (ai != bi)
        return ai < bi;

    if (ai == NodeID::placeholder){
        const Placeholder* ap = static_cast<const Placeholder*>(a.get());
        const Placeholder* bp = static_cast<const Placeholder*>(b.get());

        return ap->name <= bp->name;
    }

    if (bi == NodeID::value){
        const Value* av = static_cast<const Value*>(a.get());
        const Value* bv = static_cast<const Value*>(b.get());

        return av->value <= bv->value;
    }

    return true;
}

inline double get_value(Node& ptr)  {   return reinterpret_cast<Value*>(ptr.get())->value; }
inline bool is_leaf(Node& ptr)      {   return ptr->id <= NodeID::value;  }
inline bool is_leaf(NodeRef ptr)    {   return ptr->id <= NodeID::value;  }

inline bool is_unary(const Node& ptr)      {   return ptr->id >= NodeID::neg;  }
inline bool is_unary(const NodeRef ptr)    {   return ptr->id >= NodeID::neg;  }

inline bool is_value(Node& ptr)    {   return ptr->id == NodeID::value;  }
inline bool is_value(NodeRef ptr)  {   return ptr->id == NodeID::value;  }

// helpers
inline BinaryNode*  to_binary(const Node& ptr)     {    return reinterpret_cast<BinaryNode*> (ptr.get());  }
inline UnaryNode*   to_unary(const Node& ptr)      {    return reinterpret_cast<UnaryNode*>  (ptr.get());  }
inline Value*       to_value(const Node& ptr)      {    return reinterpret_cast<Value*>     (ptr.get());  }
inline Placeholder* to_placeholder(const Node& ptr){    return reinterpret_cast<Placeholder*>(ptr.get());  }

inline Node make_value(double val){
    return std::make_shared<Value>(val);
}

inline Node make_placeholder(const std::string& name){
    return std::make_shared<Placeholder>(name);
}

// Generate functions
#define SYMDIFF_NODES_DEFINITIONS
    //  create make_node_name() function with correct RTTI
    #define DEFINE_UNARY_NODE(__type__, __str__, __repr__) \
        inline Node make_##__str__(Node val){\
            return std::make_shared<UnaryNode>(NodeID::__str__, val);\
        }

    #define DEFINE_BINARY_NODE(__type__, __str__, __repr__) \
        inline Node make_##__str__(Node lhs, Node rhs){\
            return std::make_shared<BinaryNode>(NodeID::__str__, lhs, rhs);\
        }

    #include "Nodes.def"
#undef SYMDIFF_NODES_DEFINITIONS



}

#endif
