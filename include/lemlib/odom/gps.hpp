#pragma once

#include "odom.hpp"
#include "pros/gps.hpp"
#include "pros/imu.hpp"
#include <memory>

namespace lemlib {

class GPS : public Odom {
    public:
        /**
         * @brief Construct a new GPS object
         *
         * @param gps Shared ptr to a GPS sensor
         * @param imu Shared ptr to an IMU (optional, GPS comes with an IMU built in)
         */
        GPS(std::shared_ptr<pros::GPS> gps, std::shared_ptr<pros::IMU> imu = nullptr)
            : gps(gps),
              imu(imu) {}

        /**
         * @brief Construct a new GPS object
         *
         * @param port Port the GPS Sensor is in
         * @param imu Shared ptr to an IMU (optional, GPS comes with an IMU built in)
         */
        GPS(uint8_t port, std::shared_ptr<pros::IMU> imu = nullptr)
            : gps(std::make_shared<pros::GPS>(port)),
              imu(imu) {}

        /**
         * @brief Calibrate any sensors which may be used, or anything else that needs to be done
         * before tracking starts
         *
         * @param calibrateGyros Whether or not to calibrate gyros. True by default. Here for compatibility reasons
         */
        void calibrate(bool calibrateGyros = true) override;
        /**
         * @brief Reset the GPS Sensor with the values of the parameter GPS Sensor
         *
         * @param gps
         */
        void resetGPS(double xInitial, double yInitial, double headingInitial = 0, double xOffset = 0,
                      double yOffset = 0);
        /**
         * @brief Update odom pose
         *
         */
        void update() override;
        /**
         * @brief Get the current pose
         *
         * @return Pose current pose
         */
        Pose getPose() const;
        /**
         * @brief Set the current pose
         *
         * @param pose the new pose
         */
        void setPose(Pose pose);
    protected:
        // heading set to M_PI_2 because odom should default to IMU 0 angle
        Pose pose = Pose(0, 0, M_PI_2);

        std::shared_ptr<pros::GPS> gps;
        std::shared_ptr<pros::IMU> imu;
};

} // namespace lemlib