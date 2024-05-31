#pragma once

#include "units/Pose.hpp"
#include "units/units.hpp"

namespace lemlib {

/**
 * @struct DifferentialChassisSpeeds
 *
 * @brief represents the angular and linear velocity of the robot, as well as a weight to determine whether to
 *        prioritize linear or angular velocity when desaturating the motors
 */
struct DifferentialChassisSpeeds {
        LinearVelocity linearVelocity; /** linear velocity */
        AngularVelocity angularVelocity; /** angular velocity */
};

/**
 * @struct HolonomicChassisSpeeds
 *
 * @brief represents the speed of the x, y, and angular velocity of a holonomic drive robot
 */
struct HolonomicChassisSpeeds {
        LinearVelocity x; /** x velocity */
        LinearVelocity y; /** y velocity */
        AngularVelocity omega; /** angular velocity */
};

/**
 * @class DifferentialMotion
 *
 * @brief Abstract class which represents a motion algorithm for a differential drive robot
 *
 * We use this abstraction to allow for different motion algorithms to be used with the same interface.
 * This allows us to easily create and maintain motion algorithms while also allowing the user to
 * make custom motion algorithms without having to recompile the library.
 */
class DifferentialMotion {
    public:
        /**
         * @brief Calculates the speed of the left and right wheels of a differential drive robot
         *
         * @param pose the current pose of the robot
         * @return DifferentialChassisSpeeds the speed of the left and right wheels
         */
        virtual DifferentialChassisSpeeds calculate(units::Pose pose) = 0;
        /**
         * @brief Get whether the motion is running
         *
         * @return true the motion is running
         * @return false the motion is not running
         */
        bool isRunning();
        /**
         * @brief Destroy the Differential Motion object
         *
         */
        virtual ~DifferentialMotion();
    protected:
        bool running = true; /** whether the motion is running or not */
        const float desaturateBias = 0.5; /** weight to determine whether to prioritize linear or angular velocity when
                               desaturating the motors */
        const Length trackWidth; /** the track width of the robot */
        const LinearVelocity maxVelocity; /** the maximum velocity of the robot */
        /**
         * @brief Desaturates the motor speeds to prevent the motors from saturating
         *
         * @param speeds the speeds to desaturate
         * @return DifferentialChassisSpeeds
         */
        DifferentialChassisSpeeds desaturate(DifferentialChassisSpeeds speeds) const;
};

/**
 * @class HolonomicMotion
 *
 * @brief Abstract class which represents a motion algorithm for a holonomic drive robot
 *
 * We use this abstraction to allow for different motion algorithms to be used with the same interface.
 * This allows us to easily create and maintain motion algorithms while also allowing the user to
 * make custom motion algorithms without having to recompile the library.
 */
class HolonomicMotion {
    public:
        /**
         * @brief Calculates the field-relative speed of a holonomic drive robot
         *
         * @param pose the current pose of the robot
         * @return HolonomicChassisSpeeds the field-relative speed of the robot
         */
        virtual HolonomicChassisSpeeds calculate(units::Pose pose) = 0;
        /**
         * @brief Get whether the motion is running
         *
         * @return true the motion is running
         * @return false the motion is not running
         */
        bool isRunning();
        /**
         * @brief Destroy the Holonomic Motion object
         *
         */
        virtual ~HolonomicMotion();
    protected:
        bool running = true; /** whether the motion is running or not */
};

} // namespace lemlib