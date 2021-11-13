#include "bounce_testcase.h"

TEST_F(BounceTest, Test0) {
    ASSERT_EQ(top.p_clk__pix.get<bool>(), false);
}
