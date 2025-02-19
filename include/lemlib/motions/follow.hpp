#pragma once

#include "hardware/Motor/MotorGroup.hpp"
#include "units/Pose.hpp"
#include "hot-cold-asset/asset.hpp"

namespace lemlib {
struct FollowSettings {
        std::function<units::Pose()> poseGetter;
        lemlib::MotorGroup& leftMotors;
        lemlib::MotorGroup& rightMotors;
};

void follow(const asset& path, Length lookaheadDistance, Time timeout, bool reversed, FollowSettings settings);
} // namespace lemlib