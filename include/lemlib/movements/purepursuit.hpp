#pragma once

#include <utility>
#include <vector>
#include "lemlib/pose.hpp"
#include "lemlib/asset.hpp"
#include "lemlib/movements/movement.hpp"

namespace lemlib {
/**
 * @brief Pure Pursuit class. Derived from Movement
 */
class PurePursuit : public Movement {
    public:
        /**
         * @brief Construct a new Pure Pursuit movement
         *
         * @param trackWidth the width of the chassis. Units in inches
         * @param path const reference to the asset containing the path
         * @param lookaheadDist the lookahead distance. Units in inches. Recommended value is 15, but can be changed if
         * needed
         * @param timeout the maximum time the robot can spend moving
         * @param reversed whether the chassis should move forwards (false) or backwards (true)
         * @param maxSpeed the maximum speed the robot can move at. 127 by default
         */
        PurePursuit(float trackWidth, const asset& path, float lookaheadDist, int timeout, bool reversed, int maxSpeed);

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
        std::vector<Waypoint> path;
        Pose prevPose = Pose(0, 0, 0);
        Waypoint prevLookahead = Waypoint(0, 0);
        float trackWidth;
        int startTime;
        float lookaheadDist;
        int timeout;
        bool reversed;
        int maxSpeed;

        int compState;
        int state = 0; // 0 = in progress, 1 = done
        float dist = 0;
};
}; // namespace lemlib