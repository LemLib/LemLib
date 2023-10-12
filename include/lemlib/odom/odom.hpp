#pragma once

#include "lemlib/pose.hpp"

namespace lemlib {
class Odom {
    public:
        /**
         * @brief Calibrate the sensors used for odom
         *
         */
        virtual void calibrate() = 0;
        /**
         * @brief Update te position of odom
         *
         */
        virtual void update() = 0;

        /**
         * @brief Get the current pose
         *
         * @return Pose the current pose
         */
        Pose getPose() { return pose; };

        /**
         * @brief Set the pose
         *
         * @param pose the new pose
         */
        void setPose(Pose pose) { this->pose = pose; }
    protected:
        Pose pose = Pose(0, 0, 0);
};
} // namespace lemlib
