#ifndef SYMDIFF_INTERNAL_LLVM_HEADER_
#define SYMDIFF_INTERNAL_LLVM_HEADER_

#include "version.h"

#if defined(LLVM_FOUND) && defined(USE_LLVM_IR)
#   define SYMDIFF_LLVM
#endif

#ifdef SYMDIFF_LLVM

#include <cassert>

#include "Builder.h"
#include "Nodes.h"
#include "Context.h"

#include <iostream>

// LLVM HEADER
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Argument.h"
#include "llvm/ADT/APFloat.h"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

namespace symdiff{

/*
 * Generate a LLVM function from a Symbolic expr
 */
class LLVMGen: public Visitor
{
#define LLVM_DOUBLE(v) llvm::ConstantFP::get(builder.getDoubleTy(), v)
  public:
    LLVMGen(llvm::Function* f, llvm::LLVMContext& env, Node expr):
       ctx(env), fun(f), body(llvm::BasicBlock::Create(ctx, "fun_body", fun)),
       builder(body)
    {
        assert(fun != nullptr && "fun cant be null");

        // Generate code
        dispatch(expr);

        // f returns last expr
        builder.CreateRet(result);
    }

    static llvm::Function* make_llvm_function(const std::string& name, llvm::Module* m, llvm::LLVMContext& ctx){
        return llvm::cast<llvm::Function>(m->getOrInsertFunction(
            name, llvm::Type::getDoubleTy(ctx), nullptr));
    }

    static llvm::Function* run(llvm::Function* f, llvm::LLVMContext& ctx, Node expr){
        LLVMGen(f, ctx, expr);
        return f;
    }

    void placeholder(NodeType expr){
        // This does not work as LLVM does not update params size
        // The generated code looks fine but it fails to eval
        Placeholder* p = to_placeholder(expr);
        auto arg = new llvm::Argument(llvm::Type::getDoubleTy(ctx), p->name, fun);
        result = arg;
        args.push_back(arg);
    }

    void value(NodeType expr){
        double v = to_value(expr)->value;
        result = LLVM_DOUBLE(v);
    }

    void add(NodeType expr){
        BinaryNode* b = to_binary(expr);

        dispatch(b->lhs);
        llvm::Value* lhs = result;

        dispatch(b->rhs);
        llvm::Value* rhs = result;

        result = builder.CreateFAdd(lhs, rhs);
    }

    void mult(NodeType expr){
        BinaryNode* b = to_binary(expr);

        dispatch(b->lhs);
        llvm::Value* lhs = result;

        dispatch(b->rhs);
        llvm::Value* rhs = result;

        result = builder.CreateFMul(lhs, rhs);
    }

    // pow is not a base expr of LLVM
    void pow(NodeType)     {    result = LLVM_DOUBLE(0); }
    void inv(NodeType expr){
        UnaryNode* u = to_unary(expr);
        dispatch(u->expr);
        result = builder.CreateFDiv(LLVM_DOUBLE(1), result);
    }

    void neg(NodeType expr){
        UnaryNode* u = to_unary(expr);
        dispatch(u->expr);
        result = builder.CreateFMul(LLVM_DOUBLE(-1), result);
    }

private:
    llvm::LLVMContext& ctx;
    llvm::Function*    fun{nullptr};
    llvm::BasicBlock*  body{nullptr};
    llvm::IRBuilder<>  builder;

    llvm::Value* result{nullptr};
    std::vector<llvm::Argument*> args; // not used
};


}
/*
// This is buggy
// I think it might comes from the initialization order

#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/Interpreter.h"

#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/TargetSelect.h"

namespace symdiff{

class LLVMEval
{
public:
    LLVMEval(){
        llvm::InitializeNativeTarget();
        llvm::make_unique<llvm::Module>("symdiff", ctx);
        exec_engine = llvm::EngineBuilder(std::move(owner)).create();
    }

    ~LLVMEval(){
        delete exec_engine;
        llvm::llvm_shutdown();
    }

    llvm::Function* get_function(const std::string& name) { return owner->getFunction(name); }

    llvm::GenericValue run_function(llvm::Function* fun, std::vector<llvm::GenericValue> args){
        return exec_engine->runFunction(fun, args);
    }

    llvm::Module* module()          {   return owner.get(); }
    llvm::LLVMContext& context()    {   return ctx;         }

private:
    llvm::LLVMContext ctx;
    std::unique_ptr<llvm::Module> owner;
    llvm::ExecutionEngine* exec_engine{nullptr};
};

}*/

#endif // SYMDIFF_LLVM
#endif
