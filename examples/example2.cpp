#include "smath.h"
#include <iostream>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/Interpreter.h"

#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"

using namespace symdiff;
using namespace std;

int main(){

    auto x = make_var("x");

    Sym f = x + x * 3 + 10;

    f.print(std::cout) << std::endl;


    Context sym_ctx = {
        {"x", make_val(2)}
    };

    auto no_freev = f.substitute(sym_ctx);

    // df.reduce().print(std::cout) << std::endl;

    //
    //  Initialize LLVM
    //
    llvm::InitializeNativeTarget();
    llvm::LLVMContext ctx;

    std::unique_ptr<llvm::Module> Owner = llvm::make_unique<llvm::Module>("symdiff", ctx);
    llvm::Module *m = Owner.get();

    //
    //  Generate Function from symbolic expr
    //

    no_freev.llvm_gen("my_fun", m, ctx);

    //
    //  Create Execution Engine
    //
    llvm::ExecutionEngine* EE = llvm::EngineBuilder(std::move(Owner)).create();

    llvm::outs() << "We just constructed this LLVM module:\n\n" << *m;
    llvm::outs() << "\n\nRunning foo: \n";
    llvm::outs().flush();

    llvm::Function *fun = m->getFunction("my_fun");

    std::vector<llvm::GenericValue> noargs;
    llvm::GenericValue gv = EE->runFunction(fun, noargs);

    llvm::outs() << "Result: " << gv.FloatVal   << " \t" << 18 << "\n";
    llvm::outs() << "Result: " << gv.IntVal     << " \t" << 18 << "\n";
    llvm::outs() << "Result: " << gv.DoubleVal  << " \t" << 18 << "\n";

    delete EE;
    llvm::llvm_shutdown();
    return 0;
}
