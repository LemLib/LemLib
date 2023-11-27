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
        Imu(const pros::Imu& imu);
        /**
         * @brief Calibrate the IMU
         *
         * @param blocking whether the function should block until calibration is complete
         * @return true calibration failed
         * @return false calibration succeeded
         */
        bool calibrate(bool blocking = false) override;
        /**
         * @brief Get wether the IMU is calibrating or not
         *
         * @return true IMU is calibrating
         * @return false IMU is not calibrating
         */
        bool isCalibrating() const override;
        /**
         * @brief Get wether the IMU is calibrated or not
         *
         * @return true the IMU is calibrated
         * @return false the IMU is not calibrated
         */
        bool isCalibrated() override;
        /**
         * @brief Get whether the IMU is connected or not
         *
         * @return true the IMU is connected
         * @return false the IMU is not connected
         */
        bool isConnected() override;
        /**
         * @brief Get the heading of the IMU
         *
         * @note 0 is in the positive x direction, and heading increases counterclockwise
         *
         * @return float heading, in radians, locked from -pi to pi
         */
        float getHeading() override;
        /**
         * @brief Get the rotation of the IMU
         *
         * @note 0 is in the positive x direction, and heading increases counterclockwise
         *
         * @return float rotation, in radians
         */
        float getRotation() override;
        /**
         * @brief Set the rotation of the IMU
         *
         * @brief 0 is in the positive x direction, and heading increases counterclockwise
         *
         * @param rotation, rotation in radians
         */
        void setRotation(float rotation) const override;
        /**
         * @brief Get the port of the IMU
         *
         * @return std::uint8_t unsigned port of the gyro
         */
        std::uint8_t getPort() override;
    private:
        pros::Imu imu;
};
} // namespace lemlib
