#include "lemlib/devices/gyro/imu.hpp"

/**
 * Construct a new Imu
 *
 * Takes an unsigned integer for the port of the IMU. Pros imu is constructed
 * in an initializer list
 */
lemlib::Imu::Imu(uint8_t port)
    : imu(pros::Imu(port)) {}

/**
 * Construct a new Imu
 *
 * Takes a reference to a pros Imu to be used. Pros imu is then constructed in an
 * initializer list
 */
lemlib::Imu::Imu(pros::Imu& imu)
    : imu(imu) {}

/**
 * Calibrate the Imu
 *
 * Imu calibration should take around 2-3 seconds if blocking.
 * After imu calibration, returns whether the imu calibrated successfully or not,
 * unless the function is non-blocking, in which case success is returned whether or not
 * the imu has calibrated successfully, as we can't look into the future
 */
bool lemlib::Imu::calibrate(bool blocking) {
    if (!isConnected()) return true; // return true if imu is not connected
    imu.reset(blocking);
    if (!blocking) return false; // return false if function is non-blocking
    return !isCalibrated();
}

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
