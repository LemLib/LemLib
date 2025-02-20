#pragma once

#include "lemlib/config.hpp"
#include "hot-cold-asset/asset.hpp"

namespace lemlib {
struct FollowParams {
        bool reversed = false;
        Number lateralSlew = lateral_slew;
};

struct FollowSettings {
        Length trackWidth = track_width;
        std::function<units::Pose()> poseGetter = pose_getter;
        lemlib::MotorGroup& leftMotors = left_motors;
        lemlib::MotorGroup& rightMotors = right_motors;
};

void follow(const asset& path, Length lookaheadDistance, Time timeout, FollowParams params, FollowSettings settings);
} // namespace lemlib