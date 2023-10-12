#include "lemlib/odom/odom.hpp"

/**
 * Return current pose
 */
lemlib::Pose lemlib::Odom::getPose() { return pose; }

/**
 * Set current pose
 */
void lemlib::Odom::setPose(Pose pose) { this->pose = pose; }
