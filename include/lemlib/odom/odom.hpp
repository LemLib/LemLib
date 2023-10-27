#pragma once

#include "lemlib/pose.hpp"

namespace lemlib {
class Odom {
    public:
        /**
         * @brief Calibrate any sensors which may be used, or anything else that needs to be done
         * before tracking starts
         *
         * @param calibrateGyros Whether or not to calibrate gyros. True by default. Here for compatibility reasons
         */
        virtual void calibrate(bool calibrateGyros = true) = 0;
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
