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
        Turn(FAPID<Angle> angularPID, Angle target, int maxSpeed);

        /**
         * @brief Construct a new Turn movement
         *
         * @param angularPID the angular PID to use
         * @param target the target pose
         * @param reversed whether the robot should face the point with its back or front
         * @param maxSpeed the maximum speed the robot can turn at
         */
        Turn(FAPID<Angle> angularPID, Pose target, bool reversed, int maxSpeed);

        /**
         * @brief Update the movement
         *
         * @note This function should be called in a loop
         * @note Angle 0 is right, and increases counterclockwise
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
        FAPID<Angle> angularPID;
        std::optional<Pose> targetPose = std::nullopt;
        Pose startPose = Pose(0_in, 0_m, 0_rad);
        Angle targetHeading = 0_rad;
        bool reversed = false;
        int maxSpeed;

        int compState;
        int state = 0; // 0 = in progress, 1 = done
        Angle dist = 0_rad;
};
}; // namespace lemlib