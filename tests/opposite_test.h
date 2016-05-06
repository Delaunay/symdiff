#include "sym.h"
#include "internal/AbstractExpression.h"

#include <gtest/gtest.h>


using namespace symdiff;

TEST(opposite, eval)
{
    auto x = make_var("x");
    auto y = make_var("y");

    auto f = y - x;

    double v = full_call(f, {{"y", make_val(2)}, {"x", make_val(1)}});

    EXPECT_DOUBLE_EQ( 2. - 1., v);
}

TEST(opposite, opposite)
{
    auto x = make_var("x");

    SymExpr f = - (- x);

    EXPECT_EQ(internal::EST_Placeholder, f->get_type());
}



