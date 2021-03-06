#ifndef SYMDIFF_API_SYM_HEADER_
#define SYMDIFF_API_SYM_HEADER_

#include <memory>
#include "Context.h"

namespace symdiff{

namespace internal{
struct NodeImpl;
}


// Public API
class Sym{
public:
    typedef std::shared_ptr<internal::NodeImpl> Node;

    Sym  operator+ (Sym a);
    Sym  operator- (Sym a);
    Sym  operator/ (Sym a);
    Sym  operator* (Sym a);
    Sym  operator+ (double a);
    Sym  operator- (double a);
    Sym  operator/ (double a);
    Sym  operator* (double a);
    bool operator==(const Sym a) const;
    Sym  operator= (Sym a);

    bool match(Sym pattern);

    Sym deep_copy();
    std::ostream& pretty_print(std::ostream& out);

    double svm_eval (const NameContext& ctx);
    double rvm_eval (const NameContext& ctx);
    double full_eval(const NameContext& ctx);
    Sym partial_eval(const NameContext& ctx);
    Sym subst       (const NameContext& ctx);
    Sym derivate    (const std::string& name);

#ifdef SYMDIFF_LLVM
    Sym llvm_gen(llvm::Function* f, llvm::LLVMContext& ctx);
#endif

    static Sym make_var(double x);
    static Sym make_var(const std::string& name);
    static Sym make_any();

    // needed because of the Context
    operator Node() const { return _expr; }

    Node to_node() {    return _expr; }

private:
    Node _expr;

    Sym(Node expr):
        _expr(expr)
    {}

    friend Sym  operator+ (double b, Sym a);
    friend Sym  operator- (double b, Sym a);
    friend Sym  operator/ (double b, Sym a);
    friend Sym  operator* (double b, Sym a);
    friend Sym cond(Sym cond, Sym tr, Sym fl);
};


Sym  operator+ (double b, Sym a);
Sym  operator- (double b, Sym a);
Sym  operator/ (double b, Sym a);
Sym  operator* (double b, Sym a);

inline Sym make_var(double x)                  {   return Sym::make_var(x);   }
inline Sym make_var(const std::string& name)   {   return Sym::make_var(name);}
inline Sym make_any()                          {   return Sym::make_any();}
Sym cond(Sym cond, Sym tr, Sym fl);

inline
std::ostream& operator<<(std::ostream& out, Sym& a){    return a.pretty_print(out); }

}

#endif
