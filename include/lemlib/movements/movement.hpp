#pragma once

#include <utility>
#include "lemlib/pose.hpp"

namespace lemlib {
/**
 * @brief Abstract class for a movement
 */
class Movement {
    public:
        /**
         * @brief Get the distance travelled during the movement
         *
         * @return float
         */
        virtual float getDist() = 0;

        /**
         * @brief Update the movement
         *
         * @note This function should be called in a loop
         * @note Units are in inches and radians. Angle 0 is right, and increases counterclockwise
         *
         * @param pose the position of the chassis
         *
         * @return std::pair<int, int> left and right motor power respectively. 128 means ignore
         */
        virtual std::pair<int, int> update(Pose pose) = 0;
};
} // namespace lemlib