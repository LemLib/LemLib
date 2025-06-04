#pragma once

#include "hardware/Port.hpp"
#include "hardware/IMU/IMU.hpp"
#include "pros/imu.hpp"

namespace lemlib {
class V5InertialSensor : public IMU {
    public:
        /**
         * @brief Construct a new V5 Inertial Sensor
         *
         * @param port the port of the V5 inertial sensor
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *    // Create a new V5 Inertial Sensor on port 1
         *    lemlib::V5InertialSensor imu(1);
         * }
         * @endcode
         */
        V5InertialSensor(SmartPort port);
        /**
         * @brief Create a new V5 Inertial Sensor
         *
         * @param imu the inertial sensor
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *    // Create a new V5 Inertial Sensor on port 1
         *    lemlib::V5InertialSensor imu = lemlib::V5InertialSensor::from_pros_imu(pros::Imu(1));
         * }
         * @endcode
         */
        static V5InertialSensor from_pros_imu(pros::Imu imu);
        /**
         * @brief calibrate the V5 Inertial Sensor
         *
         * This function calibrates the IMU. Calibrating the IMU is needed before it can be used.
         * This function is non-blocking
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENXIO: The given value is not within the range of V5 ports (1-21).
         * ENODEV: The port cannot be configured as an Inertial Sensor
         * EAGAIN: The sensor is already calibrating, or time out setting the status flag.
         *
         * @return 0 upon success
         * @return INT_MAX if an error occurred, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *    lemlib::V5InertialSensor imu = pros::Imu(1);
         *
         *    if (imu.calibrate() == 0) {
         *        std::cout << "IMU calibration started" << std::endl;
         *    } else {
         *        std::cout << "IMU calibration failed" << std::endl;
         *    }
         * }
         * @endcode
         */
        int calibrate() override;
        /**
         * @brief check if the V5 Inertial Sensor is calibrated
         *
         * This function can't fail, as any failure is interpreted by either VEXOS or PROS as the IMU not being
         * calibrated. This function is non-blocking
         *
         * @return true the IMU is calibrated
         * @return false the IMU is not calibrated
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *    lemlib::V5InertialSensor imu = pros::Imu(1);
         *
         *    if (imu.isCalibrated()) {
         *        std::cout << "IMU is calibrated" << std::endl;
         *    } else {
         *        std::cout << "IMU is not calibrated" << std::endl;
         *    }
         * }
         * @endcode
         */
        int isCalibrated() override;
        /**
         * @brief check if the V5 Inertial Sensor is calibrating
         *
         * This function can't fail, as any failure is interpreted by either VEXOS or PROS as the IMU not actively
         * calibrating. This function is non-blocking
         *
         * @return true the IMU is calibrating
         * @return false the IMU is not calibrating
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *    lemlib::V5InertialSensor imu = pros::Imu(1);
         *
         *    if (imu.isCalibrating()) {
         *        std::cout << "IMU is calibrating" << std::endl;
         *    } else {
         *        std::cout << "IMU is not calibrating" << std::endl;
         *    }
         * }
         * @endcode
         */
        int isCalibrating() override;
        /**
         * @brief whether the V5 Inertial Sensor is connected
         *
         * This function can't fail, as any failure is interpreted by either VEXOS or PROS as the IMU not being
         * connected. This function is non-blocking
         *
         * @return true the IMU is connected
         * @return false the IMU is not connected
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *    lemlib::V5InertialSensor imu = pros::Imu(1);
         *
         *    if (imu.isConnected()) {
         *        std::cout << "IMU is connected" << std::endl;
         *    } else {
         *        std::cout << "IMU is not connected" << std::endl;
         *    }
         * }
         * @endcode
         */
        int isConnected() override;
        /**
         * @brief Get the rotation of the V5 Inertial Sensor
         *
         * This function returns the unbounded heading of the IMU
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENXIO: The given value is not within the range of V5 ports (1-21).
         * ENODEV: The port cannot be configured as an Inertial Sensor
         * EAGAIN: The sensor is still calibrating
         *
         * @return Angle the rotation of the IMU
         * @return INFINITY error occurred
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::V5InertialSensor imu = pros::Imu(1);
         *
         *     // wait for the IMU to calibrate
         *     imu.calibrate();
         *     while (imu.isCalibrating()) {
         *         pros::delay(10);
         *     }
         *
         *     // get the rotation of the IMU
         *     std::cout << "IMU rotation: " << to_cDeg(imu.getRotation()) << std::endl;
         * }
         * @endcode
         */
        Angle getRotation() override;
        /**
         * @brief Set the rotation of the V5 Inertial Sensor
         *
         * This function sets the unbounded heading of the IMU
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENXIO: The given value is not within the range of V5 ports (1-21).
         * ENODEV: The port cannot be configured as an Inertial Sensor
         * EAGAIN: The sensor is still calibrating
         *
         * @param rotation
         * @return int 0 success
         * @return INT_MAX error occurred
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::V5InertialSensor imu = pros::Imu(1);
         *
         *     // wait for the IMU to calibrate
         *     imu.calibrate();
         *     while (imu.isCalibrating()) {
         *         pros::delay(10);
         *     }
         *
         *     // set the rotation of the IMU
         *     imu.setRotation(90_cDeg);
         * }
         * @endcode
         */
        int setRotation(Angle rotation) override;
        /**
         * @brief Set the gyro scalar for the IMU
         *
         * This function sets the scaling factor for the IMU.
         * This is useful for when the IMU heading reading is incorrect by a constant multiplier.
         *
         * @param scalar
         * @return int 0 success
         * @return INT_MAX error occurred, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::V5InertialSensor imu = pros::Imu(1);
         *
         *     // wait for the IMU to calibrate
         *     imu.calibrate();
         *     while (imu.isCalibrating()) {
         *         pros::delay(10);
         *     }
         *
         *     // set the gyro scalar of the IMU
         *     imu.setGyroScalar(1.01);
         * }
         * @endcode
         */
        int setGyroScalar(Number scalar) override;
        /**
         * @brief Get the gyro scalar for the IMU
         *
         * This function gets the scaling factor for the IMU.
         * This is useful for when the IMU heading reading is incorrect by a constant multiplier.
         *
         * @return double gyro scalar
         * @return INT_MAX error occurred, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::V5InertialSensor imu = pros::Imu(1);
         *
         *     // wait for the IMU to calibrate
         *     imu.calibrate();
         *     while (imu.isCalibrating()) {
         *         pros::delay(10);
         *     }
         *
         *     // get the rotation of the IMU
         *     std::cout << "IMU gyro scalar: " << imu.getGyroScalar() << std::endl;
         * }
         * @endcode
         */
        Number getGyroScalar() override;
    private:
        Angle m_offset = 0_stRot;
        pros::Imu m_imu;
};
} // namespace lemlib