#include "add_test.h"
#include "mult_test.h"
#include "equiv_test.h"
#include "inverse_test.h"
#include "ln_test.h"
#include "opposite_test.h"
#include "pow_test.h"

int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();

    return 0;
}
