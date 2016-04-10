#include "sym.h"
#include "internal/Definitions.h"

#include <vector>
#include <gtest/gtest.h>

using namespace symdiff;

TEST(Add, eval)
{
    Sym x = make_var("x");
    Sym y = make_var("y");

    auto f = x + y;

    std::vector<double> values = {0, 1, 2, 3, 4, 5};

    for (int i = 0; i < 6; ++i)
        for (int j = 0; j < 6; ++j){

            double v1 = values[i];
            double v2 = values[5 - i];

            double v = full_call(f, {{"x", make_val(v1)}, {"y", make_val(v2)}});
            EXPECT_DOUBLE_EQ(v1 + v2, v);
        }
}

TEST(Add, collapse)
{
    Sym x = make_var("x");

    SymExpr f = x + x + x + x;

    EXPECT_EQ(f->get_type(), internal::EST_Mult);

    if (f->get_type() ==  internal::EST_Mult){
        internal::Mult* m = dynamic_cast<internal::Mult*> (f.get());
        EXPECT_DOUBLE_EQ(4, get_value(m->lhs()));
    }
}

TEST(Add, opposite)
{
    Sym x = make_var("x");

    SymExpr f = x - x;

    EXPECT_EQ(f->get_type(), internal::EST_Scalar);
    EXPECT_EQ(get_value(f), 0);
}

