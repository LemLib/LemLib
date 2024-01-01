#pragma once

#include <utility>
#include <optional>
#include "lemlib/pose.hpp"
#include "lemlib/pid.hpp"
#include "lemlib/movements/movement.hpp"

namespace lemlib {
/**
 * @brief Turn class. Derived from Movement
 */
class Turn : public Movement {
    public:
        /**
         * @brief Construct a new Turn movement
         *
         * @param angularPID the angular PID to use
         * @param target the target heading. Radians, 0 is right, increases counterclockwise
         * @param maxSpeed the maximum speed the robot can turn at
         */
        Turn(FAPID angularPID, float target, int maxSpeed);

        /**
         * @brief Construct a new Turn movement
         *
         * @param angularPID the angular PID to use
         * @param target the target pose
         * @param reversed whether the robot should face the point with its back or front
         * @param maxSpeed the maximum speed the robot can turn at
         */
        Turn(FAPID angularPID, Pose target, bool reversed, int maxSpeed);

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

        /**
         * @brief Get the distance travelled during the movement
         *
         * @return float
         */
        float getDist() override;
    private:
        FAPID angularPID;
        std::optional<Pose> targetPose = std::nullopt;
        Pose startPose = Pose(0, 0, 0);
        float targetHeading = 0;
        bool reversed = false;
        int maxSpeed;

        int compState;
        int state = 0; // 0 = in progress, 1 = done
        float dist = 0;
};
}; // namespace lemlib