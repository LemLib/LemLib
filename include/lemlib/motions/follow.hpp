#pragma once

#include "hardware/Motor/MotorGroup.hpp"
#include "units/Pose.hpp"
#include "hot-cold-asset/asset.hpp"

namespace lemlib {
struct FollowParams {
        bool reversed = false;
        Number lateralSlew;
};

struct FollowSettings {
        Length trackWidth;
        std::function<units::Pose()> poseGetter;
        lemlib::MotorGroup& leftMotors;
        lemlib::MotorGroup& rightMotors;
};

void follow(const asset& path, Length lookaheadDistance, Time timeout, FollowParams params, FollowSettings settings);
} // namespace lemlib