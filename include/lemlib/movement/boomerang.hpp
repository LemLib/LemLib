#pragma once

#include <utility>
#include <optional>
#include "lemlib/pose.hpp"
#include "lemlib/pid.hpp"
#include "lemlib/chassis/structs.hpp"
#include "lemlib/movement/movement.hpp"

namespace lemlib {
/**
 * @brief Boomerang movement class. Derived from Movement
 */
class Boomerang : protected Movement {
    public:
        /**
         * @brief Construct a new Boomerang movement
         *
         * @param drive drivetrain settings
         * @param angularSettings the angular PID gains for the boomerang controller
         * @param linearSettings the linear PID gains for the boomerang controller
         * @param target the target pose
         * @param timeout longest time the robot can spend moving
         * @param forwards whether the robot should move forwards or backwards. true for forwards (default), false for
         * backwards
         * @param lead the lead parameter. Determines how curved the robot will move. 0.6 by default (0 < lead < 1)
         * @param chasePower higher values make the robot move faster but causes more overshoot on turns. 0 makes it
         * default to global value
         * @param maxSpeed the maximum speed the robot can move at. 127 by default
         */
        Boomerang(Drivetrain_t drive, ChassisController_t angularSettings, ChassisController_t linearSettings,
                  Pose target, int timeout, bool forwards = true, float chasePower = 0, float lead = 0.6,
                  int maxSpeed = 127);

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
        Drivetrain_t drive;
        ChassisController_t angularSettings;
        ChassisController_t linearSettings;
        Pose target;
        int timeout;
        bool forwards;
        float chasePower;
        float lead;
        int maxSpeed;

        FAPID angularPID;
        FAPID linearPID;
};
}; // namespace lemlib