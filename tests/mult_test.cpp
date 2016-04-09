
#include "sym.h"
#include "internal/Definitions.h"

#include <gtest/gtest.h>


using namespace symdiff;

TEST(Mult, eval)
{
    auto x = make_var("x");
    auto y = make_var("y");

    auto f = x * y;

    double v = full_call(f, {{"x", make_val(2)}, {"y", make_val(3)}});

    EXPECT_DOUBLE_EQ(6, v);
}

TEST(Mult, collapse)
{
    Sym x = make_var("x");

    SymExpr f = x * x * x * x;

    EXPECT_EQ(f->get_type(), internal::EST_Pow);

    if (f->get_type() ==  internal::EST_Pow){
        internal::Pow* m = dynamic_cast<internal::Pow*> (f.get());
        EXPECT_DOUBLE_EQ(4, get_value(m->rhs()));
    }
}

TEST(Mult, inverse)
{
    Sym x = make_var("x");

    SymExpr f = x / x;

    EXPECT_EQ(f->get_type(), internal::EST_Scalar);
    EXPECT_EQ(get_value(f), 1);
}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();

    return 0;
}
