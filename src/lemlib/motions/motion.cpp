#include "lemlib/motions/motion.hpp"

namespace lemlib {
MotionState DifferentialMotion::getState() { return state; }

MotionState HolonomicMotion::getState() { return state; }
} // namespace lemlib