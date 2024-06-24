#include "lemlib/chassis/differential/motions/motion.hpp"

namespace lemlib::differential {
bool Motion::isFinished() const { return m_finished; }
} // namespace lemlib::differential
