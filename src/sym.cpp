#include "Sym.h"
#include "Builder.h"

#include "DeepCopy.h"
#include "PrettyPrint.h"
#include "StackVM.h"
#include "RegisterVM.h"
#include "FullEval.h"
#include "PartialEval.h"
#include "Substitution.h"
#include "Derivate.h"

#include "Equiv.h"

namespace symdiff{

Sym Sym::operator+ (Sym a) {    return Builder::add  (_expr, a._expr);    }
Sym Sym::operator- (Sym a) {    return Builder::minus(_expr, a._expr);    }
Sym Sym::operator/ (Sym a) {    return Builder::div  (_expr, a._expr);    }
Sym Sym::operator* (Sym a) {    return Builder::mult (_expr, a._expr);    }

Sym Sym::operator+ (double a) {    return Builder::add  (_expr, Builder::value(a));    }
Sym Sym::operator- (double a) {    return Builder::minus(_expr, Builder::value(a));    }
Sym Sym::operator/ (double a) {    return Builder::div  (_expr, Builder::value(a));    }
Sym Sym::operator* (double a) {    return Builder::mult (_expr, Builder::value(a));    }

bool Sym::operator= (Sym a) {    return equiv(_expr, a._expr);    }

Sym           Sym::deep_copy   ()                  {    return DeepCopy::run(_expr);            }
std::ostream& Sym::pretty_print(std::ostream& out) {    return PrettyPrint::run(out, _expr);    }

double Sym::svm_eval    (const NameContext& ctx)   {    return StackVM::run(ctx, _expr);        }
double Sym::rvm_eval    (const NameContext& ctx)   {    return RegisterVM::run(ctx, _expr);     }
double Sym::full_eval   (const NameContext& ctx)   {    return FullEval::run(ctx, _expr);       }
Sym    Sym::partial_eval(const NameContext& ctx)   {    return PartialEval::run(ctx, _expr);    }
Sym    Sym::subst       (const NameContext& ctx)   {    return Substitution::run(ctx, _expr);   }
Sym    Sym::derivate    (const std::string& name)   {   return Derivate::run(name, _expr); }

Sym Sym::make_var(double x)                 {   return Builder::value(x);          }
Sym Sym::make_var(const std::string& name)  {   return Builder::placeholder(name); }
}
