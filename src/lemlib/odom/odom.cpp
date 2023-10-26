#include "lemlib/odom/odom.hpp"

namespace lemlib {
/**
 * Return current pose
 */
Pose Odom::getPose() { return pose; }

/**
 * Set current pose
 */
void Odom::setPose(Pose pose) { this->pose = pose; }
}; // namespace lemlib