#pragma once

#include <utility>
#include <optional>
#include "lemlib/pose.hpp"
#include "lemlib/pid.hpp"
#include "lemlib/chassis/structs.hpp"
#include "lemlib/movement/movement.hpp"

namespace lemlib {
/**
 * @brief Turn class. Derived from Movement
 */
class Turn : protected Movement {
    public:
        /**
         * @brief Construct a new Turn movement
         *
         * @param angularSettings the angular PID gains
         * @param target the target heading. Radians, 0 is right, increases counterclockwise
         * @param timeout the maximum time the robot can spend moving
         * @param maxSpeed the maximum speed the robot can turn at. 127 by default
         */
        Turn(ChassisController_t angularSettings, float target, int timeout, int maxSpeed = 127);

        /**
         * @brief Construct a new Turn movement
         *
         * @param angularSettings the angular PID gains
         * @param target the target position
         * @param timeout the maximum time the robot can spend moving
         * @param reversed whether the robot should face the point with its back or front. False by default
         * @param maxSpeed the maximum speed the robot can turn at. 127 by default
         */
        Turn(ChassisController_t angularSettings, Pose target, int timeout, bool reversed = false, int maxSpeed = 127);

        /**
         * @brief Update the movement
         *
         * @note This function should be called in a loop
         * @note Units are in inches and radians. Angle 0 is right, and increases counterclockwise
         *
         * @param pose the position of the chassis
         *
         * @return std::pair<int, int> left and right motor power respectively. 128 means movement is done
         */
        std::pair<int, int> update(Pose pose) override;
    private:
        ChassisController_t angularSettings;
        float targetHeading;
        std::optional<Pose> targetPose = std::nullopt;
        int timeout;
        bool reversed;
        int maxSpeed;

        FAPID angularPID;
};
}; // namespace lemlib