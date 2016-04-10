#include <gtest/gtest.h>
#include "sym.h"

using namespace symdiff;

TEST(ln, eval)
{
    auto x = make_var("x");

    auto f = ln(x);

    double v = full_call(f, {{"x", make_val(2)}});

    EXPECT_DOUBLE_EQ(std::log(2), v);
}

TEST(ln, exp)
{
    auto x = make_var("x");

    SymExpr f = ln(exp(x));

    EXPECT_EQ(internal::EST_Placeholder, f->get_type());
}

