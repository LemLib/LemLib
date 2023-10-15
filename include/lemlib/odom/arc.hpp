#pragma once

#include <vector>
#include <memory>
#include "lemlib/odom/odom.hpp"
#include "lemlib/devices/trackingWheel.hpp"
#include "lemlib/devices/gyro/gyro.hpp"

namespace lemlib {
class ArcOdom : public Odom {
    public:
        /**
         * @brief Construct a new Arc Odom
         *
         * @param verticals vector containing vertical tracking wheels to be used
         * @param horizontals vector containing horizontal tracking wheels to be used
         * @param imus vector containing imus to be used
         */
        ArcOdom(std::vector<TrackingWheel>& verticals, std::vector<TrackingWheel>& horizontals,
                std::vector<std::shared_ptr<Gyro>>& gyros);

        /**
         * @brief Calibrate tracking wheels and inertial sensors
         *
         */
        void calibrate() override;
        /**
         * @brief Update odom pose
         *
         */
        void update() override;
    private:
        std::vector<TrackingWheel> verticals;
        std::vector<TrackingWheel> horizontals;
        std::vector<std::shared_ptr<Gyro>> gyros;
};
} // namespace lemlib
