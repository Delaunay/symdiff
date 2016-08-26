#ifndef SYMDIFF_API_SYM_HEADER_
#define SYMDIFF_API_SYM_HEADER_

#include <memory>

namespace symdiff{

namespace internal{
class NodeImpl;
}

// Public API
class Sym{
public:
    Sym operator+ (Sym a);
    Sym operator- (Sym a);
    Sym operator/ (Sym a);
    Sym operator* (Sym a);

private:
    std::shared_ptr<internal::NodeImpl> _expr;
};


}

#endif
