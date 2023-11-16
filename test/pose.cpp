#include <gtest/gtest.h>
#include "lemlib/pose.hpp"

TEST(PoseTest, BasicConstructor) {
    lemlib::Pose pose {1, 2};

    EXPECT_EQ(pose.x, 1);
    EXPECT_EQ(pose.y, 2);
    EXPECT_EQ(pose.theta, 0);
}

TEST(PoseTest, ConstructorWithTheta) {
    lemlib::Pose pose {1, 2, 30};

    EXPECT_EQ(pose.x, 1);
    EXPECT_EQ(pose.y, 2);
    EXPECT_EQ(pose.theta, 30);
}

TEST(PoseTest, EqualityOperator) {
    lemlib::Pose pose1 {1, 2};
    lemlib::Pose pose2 {1, 2};
    lemlib::Pose pose3 {-3, 5};

    EXPECT_EQ(pose1, pose2);
    EXPECT_NE(pose1, pose3);
    EXPECT_NE(pose2, pose3);
}

TEST(PoseTest, CopyConstructor) {
    lemlib::Pose startPose {1, 2, 30};
    lemlib::Pose pose(startPose);

    EXPECT_EQ(startPose, pose);
}

TEST(PoseTest, Addition) {
    lemlib::Pose pose1 {-8, 2, 90};
    lemlib::Pose pose2 {9, -4, 30};

    lemlib::Pose sum = pose1 + pose2;

    EXPECT_EQ(sum.x, 1);
    EXPECT_EQ(sum.y, -2);
    EXPECT_EQ(sum.theta, 90);
}

TEST(PoseTest, Subtraction) {
    lemlib::Pose pose1 {-8, 2, 90};
    lemlib::Pose pose2 {9, -4, 30};

    lemlib::Pose diff = pose1 - pose2;

    EXPECT_EQ(diff.x, -17);
    EXPECT_EQ(diff.y, 6);
    EXPECT_EQ(diff.theta, 90);
}

TEST(PoseTest, ScalarMultiplication) {
    lemlib::Pose pose1 {-8, 2, 90};
    float scalar = 2;

    lemlib::Pose product = pose1 * scalar;

    EXPECT_EQ(product.x, -16);
    EXPECT_EQ(product.y, 4);
    EXPECT_EQ(product.theta, 90);
}

TEST(PoseTest, ScalarDivision) {
    lemlib::Pose pose1 {-8, 2, 90};
    float scalar = 2;

    lemlib::Pose quotient = pose1 / scalar;

    EXPECT_EQ(quotient.x, -4);
    EXPECT_EQ(quotient.y, 1);
    EXPECT_EQ(quotient.theta, 90);
}

TEST(PoseTest, MatrixMultiplication) {
    lemlib::Pose pose1 {-8, 2, 90};
    lemlib::Pose pose2 {9, -4, 30};

    float product = pose1 * pose2;

    EXPECT_EQ(product, -8 * 9 + 2 * -4);
}