#include <gtest/gtest.h>
#include "lemlib/pose.hpp"

TEST(PoseTest, Assignment) {
  lemlib::Pose a {12,64};

  EXPECT_EQ(a.x, 12);
  EXPECT_EQ(a.y, 64);
  EXPECT_EQ(a.theta, 0);
  // // Expect two strings not to be equal.
  // EXPECT_STRNE("hello", "world");
  // // Expect equality.
  // EXPECT_EQ(7 * 6, 42);
}

TEST(PoseTest, Rotation) {
  lemlib::Pose a {12,64};
  lemlib::Pose rotateByZero = a.rotate(0);

  EXPECT_EQ(a.x, rotateByZero.x);
  EXPECT_EQ(a.y, rotateByZero.y);
  EXPECT_EQ(a.theta, rotateByZero.theta);
}