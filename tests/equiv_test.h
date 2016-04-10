#include <gtest/gtest.h>

#include "sym.h"
#include "internal/Utils.h"

using namespace symdiff;

// first test has always a higher running time
// gtest must mesure time in a weird way


// with --gtest_count=2 the second test does not have the time penalty
// so if you want to make a quick bench test better read the second measure

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



