#include <gtest/gtest.h>
#include "sym.h"

using namespace symdiff;

TEST(inverse, eval)
{
    auto x = make_var("x");
    auto y = make_var("y");

    auto f = y / x;

    double v = full_call(f, {{"y", make_val(1)}, {"x", make_val(2)}});

    EXPECT_DOUBLE_EQ( 1. / 2., v);
}

TEST(inverse, inverse)
{
    auto x = make_var("x");

    SymExpr f = one() / (one() / x);

    EXPECT_EQ(internal::EST_Placeholder, f->get_type());
}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();

    return 0;
}

