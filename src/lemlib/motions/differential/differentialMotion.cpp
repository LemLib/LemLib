#include "lemlib/motions/differential/differentialMotion.hpp"

namespace lemlib {
bool DifferentialMotion::isFinished() { return finished; }

DifferentialMotion::~DifferentialMotion() = default;
} // namespace lemlib
