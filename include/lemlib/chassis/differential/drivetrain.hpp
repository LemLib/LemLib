#pragma once

#include "pros/motor_group.hpp"
#include "units/Pose.hpp"

namespace lemlib::differential {
struct Drivetrain {
        pros::MotorGroup left;
        pros::MotorGroup right;
};

struct DrivetrainState {
        const units::Pose pose; /** pose of the chassis */
        const units::VelocityPose velocity; /** velocity of the chassis */
        const units::AccelerationPose acceleration; /** acceleration of the chassis */
};
} // namespace lemlib::differential