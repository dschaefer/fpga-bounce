#include <gtest/gtest.h>
#include <bounce_test.h>

class BounceTest: public ::testing::Test {
protected:
    cxxrtl_design::p_top__bounce top;

    BounceTest();

    void tick() {
        top.p_clk__pix.set(1U);
        top.step();
        top.p_clk__pix.set(0U);
        top.step();
    }
};
