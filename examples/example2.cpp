#include "smath.h"
#include <iostream>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/Interpreter.h"

#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace symdiff;
using namespace std;

int main(){

    auto x = make_var("x");
    auto y = make_var("y");

    Sym f = y + x * 3 + 10;

    f.print(std::cout) << std::endl;

    //
    //  Initialize LLVM
    //
    InitializeNativeTarget();
    LLVMContext ctx;

    std::unique_ptr<Module> Owner = llvm::make_unique<Module>("symdiff", ctx);
    Module *m = Owner.get();

    //
    //  Generate Function from symbolic expr
    //

    f.llvm_gen("my_fun", m, ctx);

    outs() << "We just constructed this LLVM module:\n\n" << *m;
    outs() << "\n\nRunning my_fun: \n";
    outs().flush();

    // The function is very boring since constant folding already eval it
    Function *fun = m->getFunction("my_fun");

    /*
    GenericValue arg1;    arg1.DoubleVal = 10;
    GenericValue arg2;    arg2.DoubleVal = 20;

    std::vector<GenericValue> args = {arg1, arg2};

    //
    //  Create Execution Engine
    //
    ExecutionEngine* EE = EngineBuilder(std::move(Owner)).create();
    GenericValue gv = EE->runFunction(fun, args);   // Bad alloc here

    //outs() << "Result: " << gv.FloatVal   << " \t" << 18 << "\n";
    //outs() << "Result: " << gv.IntVal     << " \t" << 18 << "\n";
    outs() << "Result: " << gv.DoubleVal  << " \t" << 18 << "\n";

    delete EE; //*/
    llvm_shutdown();
    return 0;
}
