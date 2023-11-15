#pragma once

#include <queue>
#include <utility>
#include "lemlib/pose.hpp"
#include "lemlib/pid.hpp"
#include "lemlib/movements/movement.hpp"

namespace lemlib {
/**
 * @brief Boomerang movement class. Derived from Movement
 */
class Boomerang : public Movement {
    public:
        /**
         * @brief Construct a new Boomerang movement
         *
         * @param linearPID the linear PID to use
         * @param angularPID the angular PID to use
         * @param target the target pose
         * @param forwards whether the robot should move forwards or backwards. true for forwards, false for
         * backwards
         * @param lead the lead parameter. Determines how curved the robot will move. (0 < lead < 1)
         * @param chasePower higher values make the robot move faster but causes more overshoot on turns
         * @param maxSpeed the maximum speed the robot can move at
         */
        Boomerang(FAPID<Length> linearPID, FAPID<Angle> angularPID, Pose target, bool forwards, float chasePower, float lead,
                  int maxSpeed);

        /**
         * @brief Get the distance travelled during the movement
         *
         * @return float
         */
        float getDist() override;

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
        FAPID<Angle> angularPID;
        FAPID<Length> linearPID;
        Pose target;
        Pose prevPose = Pose(0_in, 0_in, 0_deg);
        bool forwards;
        float chasePower;
        float lead;
        int maxSpeed;

        int compState;
        int state = 0; // 0 = in progress, 1 = settling, 2 = done
        Length dist = 0_in;
};
}; // namespace lemlib