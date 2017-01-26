#include <gtest/gtest.h>

#include "Sym.h"

using namespace symdiff;

TEST(equiv, same_memory)
{
    auto x = make_var("x");
    auto y = make_var("y");

    auto elem = x * y;

    EXPECT_EQ(elem, elem);
}

TEST(equiv, same_graph)
{
    auto x = make_var("x");
    auto y = make_var("y");

    auto elem1 = x * y;
    auto elem2 = x * y;

    EXPECT_EQ(elem1, elem2);
}



