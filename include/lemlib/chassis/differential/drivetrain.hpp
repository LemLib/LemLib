#pragma once

#include "pros/motor_group.hpp"
#include "units/Pose.hpp"

namespace lemlib::differential {
/**
 * @brief struct containing a left and right motor group representing a drivetrain
 */
struct Drivetrain {
        pros::MotorGroup left; /** the left side of the drivetrain */
        pros::MotorGroup right; /** the right side of the drivetrain */
};

/**
 * @brief struct containing the position, velocity, and acceleration of a drivetrain
 */
struct DrivetrainState {
        const units::Pose pose; /** pose of the chassis */
        const units::VelocityPose velocity; /** velocity of the chassis */
        const units::AccelerationPose acceleration; /** acceleration of the chassis */
};
} // namespace lemlib::differential