#include <gtest/gtest.h>
#include "sym.h"

using namespace symdiff;

TEST(exp, eval)
{
    auto x = make_var("x");

    auto f = exp(x);

    double v = full_call(f, {{"x", make_val(2)}});

    EXPECT_DOUBLE_EQ(std::exp(2), v);
}


TEST(exp, ln)
{
    auto x = make_var("x");

    SymExpr f = exp(ln(x));

    EXPECT_EQ(internal::EST_Placeholder, f->get_type());
}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();

    return 0;
}

