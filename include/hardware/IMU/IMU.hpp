#pragma once

#include "hardware/Device.hpp"
#include "units/Angle.hpp"
#include "pros/rtos.hpp"

namespace lemlib {
class IMU : public Device {
    public:
        /**
         * @brief Construct a new IMU object
         *
         * @param scalar the scalar to apply to the gyro readings. Defaults to 1.
         */
        IMU(Number scalar = 1.0);
        /**
         * @brief IMU copy constructor
         *
         * since pros::Mutex does not have a copy constructor, we need an explicit copy constructor
         *
         * @param other the imu to copy
         */
        IMU(const IMU& other);
        /**
         * @brief calibrate the IMU
         *
         * This function calibrates the IMU. Usually its necessary to calibrate an IMU before using it.
         * This function is non-blocking.
         *
         * @return 0 success
         * @return INT_MAX error occurred, setting errno
         */
        virtual int32_t calibrate() = 0;
        /**
         * @brief check if the IMU is calibrated
         *
         * @return true the IMU is calibrated
         * @return false the IMU is not calibrated
         * @return INT_MAX error occurred, setting errno
         */
        virtual int32_t isCalibrated() const = 0;
        /**
         * @brief check if the IMU is calibrating
         *
         * @return true the IMU is calibrating
         * @return false the IMU is not calibrating
         * @return INT_MAX error occurred, setting errno
         */
        virtual int32_t isCalibrating() const = 0;
        /**
         * @brief Get the rotation of the IMU
         *
         * This function returns the unbounded heading of the IMU
         *
         * @return Angle the rotation of the IMU
         * @return INFINITY error occurred, setting errno
         */
        virtual Angle getRotation() const = 0;
        /**
         * @brief Set the rotation of the IMU
         *
         * This function sets the unbounded heading of the IMU
         *
         * @param rotation
         * @return int 0 success
         * @return INT_MAX error occurred, setting errno
         */
        virtual int32_t setRotation(Angle rotation) = 0;
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
        virtual int32_t setGyroScalar(Number scalar);
        /**
         * @brief Get the gyro scalar for the IMU
         *
         * This function gets the scaling factor for the IMU.
         * This is useful for when the IMU heading reading is incorrect by a constant multiplier.
         *
         * @return Number gyro scalar
         * @return INFINITY error occurred, setting errno
         */
        virtual Number getGyroScalar() const;
        virtual ~IMU() = default;
    protected:
        mutable pros::Mutex m_mutex;
        Number m_gyroScalar;
};
} // namespace lemlib
