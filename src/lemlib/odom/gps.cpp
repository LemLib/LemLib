#include "lemlib/odom/gps.hpp"

namespace lemlib {

void GPS::calibrate(bool calibrateGyros) {
    if (calibrateGyros) { imu->reset(); }
}

void GPS::resetGPS(double xInitial, double yInitial, double headingInitial, double xOffset, double yOffset) {
    this->gps->initialize_full(xInitial, yInitial, headingInitial, xOffset, yOffset);
}

void GPS::update() {
    // update pose
    pose = Pose(gps->get_position().x, gps->get_position().y, imu->get_rotation());
}

Pose GPS::getPose() const { return pose; }

void GPS::setPose(Pose pose) { this->pose = pose; }

} // namespace lemlib
