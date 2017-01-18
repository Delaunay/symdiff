#include "BenchTimer.h"
#include <iostream>

#include "../src/Context.h"
#include "../src/Sym.h"
#include "../src/RegisterVM.h"
#include "../src/StackVM.h"
#include "../src/PrettyPrint.h"

using namespace symdiff;

// 2^i mult n * (n - 1) * ....
Sym make_balanced_expr(double n, int i){

    if (i == 0)
        return make_var(n);

    auto a = make_balanced_expr(n, i - 1);

    //n = n - std::pow(2, i - 1);

    auto b = make_balanced_expr(n, i - 1);

    return a + b;
}

Sym make_unbalanced_expr(double n, int i){
    Sym a = make_var(0);

    for(int j = std::pow(2, i); j > 0; --j/*, --n*/){
        a = make_var(n) + a;
    }

    return a;
}

template<typename T>
std::string to_string(T num, std::size_t trunc = 4, std::size_t width = 2){
    // %[flags][width][.precision][length]

    std::string format = "% #" + std::to_string(width) +
                           "." + std::to_string(trunc) + "f";
    char c[] = "                                  ";
    sprintf(c, format.c_str(), num);
    return std::string(c);

//    double mult = std::pow(10, 4);
//    int trunc_num = num * mult;
//    return std::to_string(trunc_num / mult);
}

std::ostream& operator<< (std::ostream& out, bench::BenchTimer& t){
    out << to_string(t.best(),  4) << "\t"
        << to_string(t.worst(), 4) << "\t"
        << to_string(t.total(), 4) << "\t"
        << to_string(t.sd(),    4);

    return out;
}

int main()
{
    NameContext env;

    int depth = 6;

    auto bl  = make_balanced_expr(2, depth);
    auto ubl = make_unbalanced_expr(2, depth);

    int tries = 1000;
    int rep   = 1000;

    bench::BenchTimer bpartial; BENCH(bpartial, tries, rep, bl.partial_eval(env));
    bench::BenchTimer bfull;    BENCH(bfull, tries, rep, bl.full_eval(env));
    bench::BenchTimer bsvm;     BENCH(bsvm, tries, rep, bl.svm_eval(env));
    bench::BenchTimer brvm;     BENCH(brvm, tries, rep, bl.rvm_eval(env));

    bench::BenchTimer upartial; BENCH(upartial, tries, rep, ubl.partial_eval(env));
    bench::BenchTimer ufull;    BENCH(ufull, tries, rep, ubl.full_eval(env));
    bench::BenchTimer usvm;     BENCH(usvm, tries, rep, ubl.svm_eval(env));
    bench::BenchTimer urvm;     BENCH(urvm, tries, rep, ubl.rvm_eval(env));

    auto bln = bl.to_node();
    auto uln = ubl.to_node();

//    std::cout << "  Balanced: ";
//    PolishPrint::run(std::cout, bln) << std::endl;

//    std::cout << "Unbalanced: ";
//    PolishPrint::run(std::cout, uln) << std::endl;

    std::cout << "  Balanced: " << bl.full_eval(env) << std::endl;
    std::cout << "Unbalanced: " << ubl.full_eval(env) << std::endl;

    // Compile
    auto irvmbl = RegisterGenerator::run(bln);
    auto irvmul = RegisterGenerator::run(uln);
    auto isvmbl = StackGenerator::run(bln);
    auto isvmul = StackGenerator::run(uln);

    // Compile Time
    bench::BenchTimer bcrvm; BENCH(bcrvm, tries, rep, RegisterGenerator::run(bln));
    bench::BenchTimer ucrvm; BENCH(ucrvm, tries, rep, RegisterGenerator::run(uln));
    bench::BenchTimer bcsvm; BENCH(bcsvm, tries, rep, StackGenerator::run(bln));
    bench::BenchTimer ucsvm; BENCH(ucsvm, tries, rep, StackGenerator::run(uln));

    // Eval Time
    bench::BenchTimer bervm; BENCH(bervm, tries, rep, RegisterVM::run(env, irvmbl));
    bench::BenchTimer uervm; BENCH(uervm, tries, rep, RegisterVM::run(env, irvmul));
    bench::BenchTimer besvm; BENCH(besvm, tries, rep, StackVM::run(env, isvmbl));
    bench::BenchTimer uesvm; BENCH(uesvm, tries, rep, StackVM::run(env, isvmul));

    std::cout << "Partial  : " << bpartial << std::endl;
    std::cout << "Full     : " << bfull << std::endl;
    std::cout << "(SVM) C+E: " << bsvm  << std::endl;
    std::cout << "(RVM) C+E: " << brvm  << std::endl;
    std::cout << "(SVM) C  : " << bcsvm << std::endl;
    std::cout << "(RVM) C  : " << bcrvm << std::endl;
    std::cout << "(SVM) E  : " << besvm << std::endl;
    std::cout << "(RVM) E  : " << bervm << std::endl;

    std::cout << std::string(80, '-') << std::endl;

    std::cout << "Partial  : " << upartial << std::endl;
    std::cout << "Full     : " << ufull << std::endl;
    std::cout << "(SVM) C+E: " << usvm  << std::endl;
    std::cout << "(RVM) C+E: " << urvm  << std::endl;
    std::cout << "(SVM) C  : " << ucsvm << std::endl;
    std::cout << "(RVM) C  : " << ucrvm << std::endl;
    std::cout << "(SVM) E  : " << uesvm << std::endl;
    std::cout << "(RVM) E  : " << uervm << std::endl;

    std::cout << std::string(80, '-') << std::endl;
    std::cout << "Unbalanced / Balanced" << std::endl;
    std::cout << std::string(80, '-') << std::endl;
    std::cout << "Partial  : " << upartial / bpartial << std::endl;
    std::cout << "Full     : " << ufull / bfull << std::endl;
    std::cout << "(SVM) C+E: " << usvm  / bsvm  << std::endl;
    std::cout << "(RVM) C+E: " << urvm  / brvm  << std::endl;
    std::cout << "(SVM) C  : " << ucsvm / bcsvm << std::endl;
    std::cout << "(RVM) C  : " << ucrvm / bcrvm << std::endl;
    std::cout << "(SVM) E  : " << uesvm / besvm << std::endl;
    std::cout << "(RVM) E  : " << uervm / bervm << std::endl;

    return 0;
}
