/*
 *      Generate LLVM-IR from a symbolic expression
 *
 *      Expr:
 *      10 + y + 3 * x
 *
 *      LLVM-IR:
 *      define double @my_fun(double %y, double %x) {
 *      fun_body:
 *         %0 = fmul double 3.000000e+00, %x
 *         %1 = fadd double %y, %0
 *         %2 = fadd double 1.000000e+01, %1
 *         ret double %2
 *      }
 *
 *
 */

#include "Builder.h"
#include "PrettyPrint.h"
#include "LLVMGen.h"

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

    auto x = Builder::placeholder("x");
    auto y = Builder::placeholder("y");
    auto mult = Builder::mult;
    auto add  = Builder::add;
    auto three = Builder::value(3);
    auto ten   = Builder::value(10);

    auto f = add(y, add(mult(x, three), ten));

    PrettyPrint::run(std::cout, f) << std::endl;

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
    auto fun = LLVMGen::make_llvm_function("my_fun", m, ctx);
               LLVMGen::run(fun, ctx, f);

    outs() << "We just constructed this LLVM module:\n\n" << *m;
    outs() << "\n\nRunning my_fun: \n";
    outs().flush();

    ///*
    GenericValue arg1;    arg1.DoubleVal = 1;
    GenericValue arg2;    arg2.DoubleVal = 2;

    std::vector<GenericValue> args = {arg1, arg2};

    //
    //  Create Execution Engine
    //
    ExecutionEngine* EE = EngineBuilder(std::move(Owner)).create();
    // Assertion failed: Index < Length && "Invalid index!", file C:\Users\newton\source\llvm\include\llvm/ADT/ArrayRef.h, line 186
    // Used to work on linux
    GenericValue gv = EE->runFunction(fun, args);

    //outs() << "Result: " << gv.FloatVal   << " \t" << 18 << "\n";
    //outs() << "Result: " << gv.IntVal     << " \t" << 18 << "\n";
    outs() << "Result: " << gv.DoubleVal  << " \t" << 18 << "\n";

    delete EE; //*/
    llvm_shutdown();
    return 0;
}
