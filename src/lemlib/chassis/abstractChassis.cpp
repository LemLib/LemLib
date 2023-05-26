//
// Created by Rocky Chen on 2023/5/26.
//
#include "pros/misc.hpp"
#include "lemlib/chassis/abstractChassis.hpp"
#include "lemlib/chassis/odom.hpp"

namespace lemlib {

    /**
     * @brief Set the Pose object
     *
     * @param x new x value
     * @param y new y value
     * @param theta new theta value
     * @param radians true if theta is in radians, false if not. False by default
 */
    void abstractChassis::setPose(double x, double y, double theta, bool radians) {
        lemlib::setPose(lemlib::Pose(x, y, theta), radians);
    }
    /**
     * @brief Set the pose of the chassis
     *
     * @param Pose the new pose
     * @param radians whether pose theta is in radians (true) or not (false). false by default
     */
    void abstractChassis::setPose(Pose pose, bool radians) { lemlib::setPose(pose, radians); }
    /**
     * @brief Get the pose of the chassis
     *
     * @param radians whether theta should be in radians (true) or degrees (false). false by default
     * @return Pose
     */
    Pose abstractChassis::getPose(bool radians) { return lemlib::getPose(radians); }

} // lemlib