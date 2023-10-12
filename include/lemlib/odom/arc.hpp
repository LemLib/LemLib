#pragma once

#include <vector>
#include "pros/imu.hpp"
#include "lemlib/devices/trackingWheel.hpp"
#include "lemlib/odom/odom.hpp"

namespace lemlib {
class Arc : public Odom {
    public:
        /**
         * @brief Create a new Arc odom
         *
         * @param verticals vector of vertical tracking wheels
         * @param horizontals vector of horizontal tracking wheels
         * @param imus vector of imus
         */
        Arc(std::vector<TrackingWheel>& verticals, std::vector<TrackingWheel>& horizontals,
            std::vector<pros::Imu>& imus);
        /**
         * @brief Calibrate the sensors used for odom
         *
         */
        void calibrate(int timeout = 3000) override;
        /**
         * @brief Update te position of odom
         *
         */
        void update() override;
    private:
        std::vector<TrackingWheel> verticals;
        std::vector<TrackingWheel> horizontals;
        std::vector<pros::Imu> imus;
};
} // namespace lemlib
