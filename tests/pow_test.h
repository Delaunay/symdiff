#include <gtest/gtest.h>
#include "sym.h"

using namespace symdiff;

TEST(pow, eval)
{
    auto x = make_var("x");
    auto y = make_var("y");

    auto f = pow(y, x);

    double v = full_call(f, {{"y", make_val(3)}, {"x", make_val(3)}});

    EXPECT_DOUBLE_EQ( 27., v);
}

