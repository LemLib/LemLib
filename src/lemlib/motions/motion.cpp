#include "lemlib/motions/motion.hpp"

namespace lemlib {
bool DifferentialMotion::isRunning() { return running; }

bool HolonomicMotion::isRunning() { return running; }
} // namespace lemlib