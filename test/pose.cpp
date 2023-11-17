#include <gtest/gtest.h>
#include "lemlib/pose.hpp"
#include <cmath>
#include <vector>

class PoseTest : public testing::TestWithParam<float> {};

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

TEST(PoseTest, Lerp) {
    lemlib::Pose pose1 {-3, 8, 90};
    lemlib::Pose pose2 {9, -8, 30};
    lemlib::Pose pose2WithPose1Theta {pose2.x, pose2.y, pose1.theta};

    lemlib::Pose lerpStart = pose1.lerp(pose2, 0);
    lemlib::Pose lerpEnd = pose1.lerp(pose2, 1);
    lemlib::Pose lerpHalf = pose1.lerp(pose2, 0.5);

    EXPECT_EQ(lerpStart, pose1);
    EXPECT_EQ(lerpEnd, pose2WithPose1Theta);
    EXPECT_EQ(lerpHalf, lemlib::Pose(3, 0, 90));
}

TEST(PoseTest, Distance) {
    lemlib::Pose pose1 {-1, 3, 90};
    lemlib::Pose pose2 {2, -1, 30};

    float distance = pose1.distance(pose2);

    EXPECT_EQ(distance, 5);
}

TEST(PoseTest, Angle) {
    lemlib::Pose origin {0, 0};

    lemlib::Pose east {1, 0};
    lemlib::Pose west {-1, 0};
    lemlib::Pose north {0, 1};
    lemlib::Pose south {0, -1};
    lemlib::Pose nEast {1, 1};
    lemlib::Pose sEast {1, -1};
    lemlib::Pose nWest {-1, 1};
    lemlib::Pose sWest {-1, -1};

    EXPECT_NEAR(origin.angle(east), 0, FLT_EPSILON);
    EXPECT_NEAR(origin.angle(nEast), M_PI / 4, FLT_EPSILON);
    EXPECT_NEAR(origin.angle(north), M_PI / 2, FLT_EPSILON);
    EXPECT_NEAR(origin.angle(nWest), 3 * M_PI / 4, FLT_EPSILON);
    EXPECT_NEAR(origin.angle(west), M_PI, FLT_EPSILON);
    EXPECT_NEAR(origin.angle(sWest), -3 * M_PI / 4, FLT_EPSILON);
    EXPECT_NEAR(origin.angle(south), -M_PI / 2, FLT_EPSILON);
    EXPECT_NEAR(origin.angle(sEast), -M_PI / 4, FLT_EPSILON);
}

TEST(PoseTest, Rotate) {
    lemlib::Pose origin {0, 0};
    lemlib::Pose pose {1, 0, 0};

    for (auto angle : std::vector<float>({0, 1 / 6, 1 / 4, 1 / 3, 1 / 2, 2 / 3, 3 / 4, 5 / 6})) {
        lemlib::Pose rotated = pose.rotate(angle * M_PI);
        lemlib::Pose rotatedNegative = pose.rotate(-angle * M_PI);

        EXPECT_NEAR(origin.angle(rotated), angle * M_PI, FLT_EPSILON);
        EXPECT_NEAR(origin.angle(rotatedNegative), -angle * M_PI, FLT_EPSILON);
    }
}
