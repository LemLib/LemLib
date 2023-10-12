#pragma once

#include "lemlib/pose.hpp"

namespace lemlib {
class Odom {
    public:
        /**
         * @brief Update odom pose
         *
         */
        virtual void update() = 0;
        /**
         * @brief Get the current pose
         *
         * @return Pose current pose
         */
        Pose getPose();
        /**
         * @brief Set the current pose
         *
         * @param pose the new pose
         */
        void setPose(Pose pose);
    protected:
        Pose pose = Pose(0, 0, 0);
};
} // namespace lemlib
