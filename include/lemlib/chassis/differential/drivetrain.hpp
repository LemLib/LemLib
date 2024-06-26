#pragma once

#include "pros/motor_group.hpp"
#include "units/Pose.hpp"

namespace lemlib::differential {
/**
 * @brief struct containing a left and right motor group representing a drivetrain
 */
struct Drivetrain {
        /** the left side of the drivetrain */
        pros::MotorGroup left;
        /** the right side of the drivetrain */
        pros::MotorGroup right;
};

/**
 * @brief struct containing the position, velocity, and acceleration of a chassis
 */
struct ChassisState {
        /** pose of the chassis */
        const units::Pose pose;
        /** velocity of the chassis */
        const units::VelocityPose velocity;
        /** acceleration of the chassis */
        const units::AccelerationPose acceleration;
};
} // namespace lemlib::differential