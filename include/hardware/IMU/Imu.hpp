#pragma once

#include "units/Angle.hpp"
#include "units/units.hpp"

namespace lemlib {
class Imu {
    public:
        /**
         * @brief calibrate the IMU
         *
         * This function calibrates the IMU. Usually its necessary to calibrate an IMU before using it.
         * This function is non-blocking.
         *
         * @return 0 success
         * @return INT_MAX error occurred, setting errno
         */
        virtual int calibrate() = 0;
        /**
         * @brief check if the IMU is calibrated
         *
         * @return true the IMU is calibrated
         * @return false the IMU is not calibrated
         * @return INT_MAX error occurred, setting errno
         */
        virtual int isCalibrated() = 0;
        /**
         * @brief check if the IMU is calibrating
         *
         * @return true the IMU is calibrating
         * @return false the IMU is not calibrating
         * @return INT_MAX error occurred, setting errno
         */
        virtual int isCalibrating() = 0;
        /**
         * @brief whether the IMU is connected
         *
         * @return true the IMU is connected
         * @return false the IMU is not connected
         * @return INT_MAX error occurred, setting errno
         */
        virtual int isConnected() = 0;
        /**
         * @brief Get the rotation of the IMU
         *
         * This function returns the unbounded heading of the IMU
         *
         * @return Angle the rotation of the IMU
         * @return INFINITY error occurred, setting errno
         */
        virtual Angle getRotation() = 0;
        /**
         * @brief Set the rotation of the IMU
         *
         * This function sets the unbounded heading of the IMU
         *
         * @param rotation
         * @return int 0 success
         * @return INT_MAX error occurred, setting errno
         */
        virtual int setRotation(Angle rotation) = 0;
        /**
         * @brief Set the gyro scalar for the IMU
         *
         * This function sets the scaling factor for the IMU.
         * This is useful for when the IMU heading reading is incorrect by a constant multiplier.
         *
         * @param scalar
         * @return int 0 success
         * @return INT_MAX error occurred, setting errno
         */
        virtual int setGyroScalar(double scalar);
        /**
         * @brief Get the gyro scalar for the IMU
         *
         * This function gets the scaling factor for the IMU.
         * This is useful for when the IMU heading reading is incorrect by a constant multiplier.
         *
         * @return double gyro scalar
         * @return INFINITY error occurred, setting errno
         */
        virtual double getGyroScalar();
        virtual ~Imu() = default;
    protected:
        double m_gyroScalar = 1.0;
};
} // namespace lemlib
