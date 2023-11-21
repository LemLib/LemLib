#include <gtest/gtest.h>
#include <cmath>
#include <vector>

#include "lemlib/util.hpp"
#include "lemlib/pose.hpp"

TEST(UtilsTest, slewUnderLimit) {
    float output = lemlib::slew(50, 5, 100);

    EXPECT_EQ(output, 50);
}

TEST(UtilsTest, slewOverLmimit) {
    float output = lemlib::slew(500, 0, 100);

    EXPECT_EQ(output, 100);
}

TEST(UtilsTest, angleErrorDegrees) {
    float output = lemlib::angleError(90, 45, false);
    EXPECT_EQ(output, 45);

    output = lemlib::angleError(315, 45, false);
    EXPECT_EQ(output, -90);
}

TEST(UtilsTest, angleErrorRadians) {
    float output = lemlib::angleError(M_PI / 2, M_PI / 4, true);
    EXPECT_FLOAT_EQ(output, (M_PI / 4));

    output = lemlib::angleError(((7 * M_PI) / 4), (M_PI / 4), true);
    EXPECT_FLOAT_EQ(output, -M_PI / 2);
}

TEST(UtilsTest, avg) {
    // bob
    float output = lemlib::avg({1, 2, 3, 4});
    EXPECT_FLOAT_EQ(output, 2.5);
}

TEST(UtilsTest, ema) {
    float output = lemlib::ema(127, 0, 0.5);
    EXPECT_FLOAT_EQ(output, 63.5);
}
