#pragma once

#include <vector>
#include <memory>
#include "lemlib/odom/odom.hpp"
#include "lemlib/devices/trackingWheel.hpp"
#include "lemlib/devices/gyro/gyro.hpp"

namespace lemlib {
class DifferentialArc : public Odom {
    public:
        /**
         * @brief Construct a new Arc Odom
         *
         * @param verticals vector containing vertical tracking wheels to be used
         * @param horizontals vector containing horizontal tracking wheels to be used
         * @param drivetrain vector containing drivetrain motors as tracking wheels
         * @param imus vector containing imus to be used
         */
        DifferentialArc(std::vector<TrackingWheel>& verticals, std::vector<TrackingWheel>& horizontals,
                        std::vector<TrackingWheel>& drivetrain, std::vector<std::shared_ptr<Gyro>>& gyros);

        /**
         * @brief Calibrate tracking wheels and inertial sensors
         *
         * @param calibrateGyros Whether or not to calibrate gyros. True by default. Here for compatibility reasons
         */
        void calibrate(bool calibrateGyros = true) override;
        /**
         * @brief Update odom pose
         *
         */
        void update() override;
    private:
        std::vector<TrackingWheel> verticals;
        std::vector<TrackingWheel> horizontals;
        std::vector<TrackingWheel> drivetrain;
        std::vector<std::shared_ptr<Gyro>> gyros;
};
} // namespace lemlib
