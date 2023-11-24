#include <cmath>
#include "lemlib/util.hpp"
#include "lemlib/devices/gyro/imu.hpp"

namespace lemlib {
/**
 * Construct a new Imu
 *
 * Takes an unsigned integer for the port of the IMU. Pros imu is constructed
 * in an initializer list
 */
Imu::Imu(uint8_t port)
    : imu(pros::Imu(port)) {}

/**
 * Construct a new Imu
 *
 * Takes a reference to a pros Imu to be used. Pros imu is then constructed in an
 * initializer list
 */
Imu::Imu(const pros::Imu& imu)
    : imu(imu) {}

/**
 * Calibrate the Imu
 *
 * Imu calibration should take around 2-3 seconds if blocking.
 * After imu calibration, returns whether the imu calibrated successfully or not,
 * unless the function is non-blocking, in which case success is returned whether or not
 * the imu has calibrated successfully, as we can't look into the future
 */
bool Imu::calibrate(bool blocking) {
    if (!isConnected()) return true; // return true if imu is not connected
    imu.reset(blocking);
    if (!blocking) return false; // return false if function is non-blocking
    return !isCalibrated();
}

/**
 * return wether the IMU is calibrating or not.
 *
 * Just a wrapper for the pros::Imu::is_calibrating() function
 */
bool Imu::isCalibrating() const { return imu.is_calibrating(); }

/**
 * return whether the IMU has been calibrated
 *
 * This function checks if the Imu is connected, is not calibrating,
 */
bool Imu::isCalibrated() { return isConnected() && !imu.is_calibrating() && !std::isinf(imu.get_heading()); }

/**
 * return whether the IMU is installed
 *
 * Just a wrapper for the pros::Imu::is_installed() function
 */
bool Imu::isConnected() { return imu.is_installed(); }

/**
 * return the heading of the imu in radians and in standard position
 */
float Imu::getHeading() {
    float heading = std::fmod(getRotation(), 2 * M_PI);
    if (heading < 0) heading += 2 * M_PI;
    return heading;
}

/**
 * Get the rotation of the imu in radians and in standard position
 */
float Imu::getRotation() {
    const float rotation = M_PI_2 - degToRad(imu.get_rotation());
    lastAngle = rotation;
    return rotation;
}

/**
 * Set the rotation of the imu in radians and in standard position
 */
void Imu::setRotation(float orientation) const { imu.set_rotation(radToDeg(M_PI - radToDeg(orientation))); }

/**
 * Wrapper function for pros::Imu.get_port()
 */
std::uint8_t Imu::getPort() { return imu.get_port(); }
}; // namespace lemlib