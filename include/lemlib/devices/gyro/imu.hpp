#pragma once

#include <cstdint>
#include "pros/imu.hpp"
#include "lemlib/devices/gyro/gyro.hpp"

namespace lemlib {
class Imu : public Gyro {
    public:
        /**
         * @brief Construct a new Imu
         *
         * @param port the port of the IMU
         */
        Imu(uint8_t port);
        /**
         * @brief Construct a new Imu
         *
         * @param imu reference to the pros imu to use
         */
        Imu(pros::Imu& imu);
        /**
         * @brief Calibrate the IMU
         *
         * @param blocking whether the function should block until calibration is complete
         * @return true calibration failed
         * @return false calibration succeeded
         */
        bool calibrate(bool blocking = false);
        /**
         * @brief Get wether the IMU is calibrating or not
         *
         * @return true IMU is calibrating
         * @return false IMU is not calibrating
         */
        bool isCalibrating();
        /**
         * @brief Get wether the IMU is calibrated or not
         *
         * @return true the IMU is calibrated
         * @return false the IMU is not calibrated
         */
        bool isCalibrated();
        /**
         * @brief Get whether the IMU is connected or not
         *
         * @return true the IMU is connected
         * @return false the IMU is not connected
         */
        bool isConnected();
        /**
         * @brief Get the heading of the gyro
         *
         * @note 0 is in the positive x direction, and heading increases counterclockwise
         *
         * @return float heading, in radians, locked from 0-2pi
         */
        float getHeading();
        /**
         * @brief Get the orientation of the gyro
         *
         * @note 0 is in the positive x direction, and heading increases counterclockwise
         *
         * @return float orientation, in radians
         */
        float getOrientation();
        /**
         * @brief Set the orientation of the gyro
         *
         * @brief 0 is in the positive x direction, and heading increases counterclockwise
         *
         * @param orientation orientation, in radians
         */
        void setOrientation(float orientation);
    private:
        pros::Imu imu;
};
} // namespace lemlib