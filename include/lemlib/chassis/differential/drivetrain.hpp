#pragma once

#include "units/Pose.hpp"
#include "hardware/Motor/MotorGroup.hpp"

namespace lemlib::differential {
/**
 * @brief struct containing a left and right motor group representing a drivetrain
 */
struct Drivetrain {
        /** the left side of the drivetrain */
        lemlib::MotorGroup left;
        /** the right side of the drivetrain */
        lemlib::MotorGroup right;
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