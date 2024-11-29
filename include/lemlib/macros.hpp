#pragma once

#include "pros/rtos.hpp" // IWYU pragma: keep
#include "lemlib/MotionHandler.hpp" // IWYU pragma: keep

/**
 * @brief this macro can be used to greatly simplify passing motion algorithms to the motion handler
 *
 * @b Example:
 * @code {.cpp}
 * void autonomous() {
 *   MOVE_CUSTOM(simpleMotion())
 * }
 * @endcode
 */
#define MOVE_CUSTOM(f) lemlib::motion_handler::move([&] { f; });

/**
 * @brief this macro can be used to wait until a condition has been met
 *
 * @b Example:
 * @code {.cpp}
 * void initialize() {
 *   pros::Imu imu(1); // inertial sensor on port 1
 *   // start calibrating the inertial sensor
 *   imu.reset();
 *   // wait until the inertial sensor has finished calibrating
 *   WAIT_UNTIL(!imu.is_calibrating())
 *   std::cout << "IMU calibrated" << std::endl;
 * }
 * @endcode
 */
#define WAIT_UNTIL(c)                                                                                                  \
    while (!(c)) pros::delay(5);
