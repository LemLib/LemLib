//
// Created by Rocky Chen on 2023/5/26.
//
#include "lemlib/chassis/abstractChassis.hpp"
#include "lemlib/chassis/odom.hpp"
#include "pros/misc.hpp"

namespace lemlib {

AbstractChassis::AbstractChassis(ChassisController_t lateralSettings, ChassisController_t angularSettings,
                                 OdomSensors_t odomSensors)
    : lateralSettings(lateralSettings), angularSettings(angularSettings), odomSensors(odomSensors) {};

AbstractChassis::AbstractChassis(ChassisController_t lateralSettings, ChassisController_t angularSettings,
                                 ChassisController_t strafeSettings, OdomSensors_t odomSensors)
    : lateralSettings(lateralSettings), angularSettings(angularSettings), strafeSettings(strafeSettings),
      odomSensors(odomSensors) {};

void AbstractChassis::calibrate() {
    if (this->odomSensors.imu != nullptr) {
        this->odomSensors.imu->reset(true);
        // keep on calibrating until it calibrates successfully
        while (errno == PROS_ERR || errno == ENODEV || errno == ENXIO) {
            pros::c::controller_rumble(pros::E_CONTROLLER_MASTER, "---");
            odomSensors.imu->reset(true);
            pros::delay(10);
        }
    }
    if (odomSensors.horizontal1 != nullptr) odomSensors.horizontal1->reset();
    if (odomSensors.horizontal2 != nullptr) odomSensors.horizontal2->reset();
}

/**
 * @brief Set the Pose object
 *
 * @param x new x value
 * @param y new y value
 * @param theta new theta value
 * @param radians true if theta is in radians, false if not. False by default
 */
void AbstractChassis::setPose(double x, double y, double theta, bool radians) {
    lemlib::setPose(lemlib::Pose(x, y, theta), radians);
}

/**
 * @brief Set the pose of the chassis
 *
 * @param Pose the new pose
 * @param radians whether pose theta is in radians (true) or not (false). false
 * by default
 */
void AbstractChassis::setPose(Pose pose, bool radians) { lemlib::setPose(pose, radians); }

/**
 * @brief Get the pose of the chassis
 *
 * @param radians whether theta should be in radians (true) or degrees (false).
 * false by default
 * @return Pose
 */
Pose AbstractChassis::getPose(bool radians) { return lemlib::getPose(radians); }

} // namespace lemlib
