#include "lemlib/motions/motion.hpp"
#include "units/units.hpp"

namespace lemlib {
bool DifferentialMotion::isRunning() const { return running; }

DifferentialChassisSpeeds DifferentialMotion::desaturate(DifferentialChassisSpeeds speeds) const {
    // get throttle and turn velocity
    LinearVelocity throttle = speeds.linearVelocity;
    LinearVelocity turn = (speeds.angularVelocity * (trackWidth / 2)).val();
    // desaturate if necessary
    if (units::abs(throttle) + units::abs(turn) > maxVelocity) {
        const LinearVelocity prevThrottle = throttle;
        throttle *= (1 - desaturateBias * std::abs(turn.val() / maxVelocity.val()));
        turn *= (1 - (1 - desaturateBias) * std::abs(prevThrottle.val() / maxVelocity.val()));
    }
    // return desaturated speeds
    return {throttle, turn.val() / (trackWidth / 2).val()};
}

bool HolonomicMotion::isRunning() const { return running; }
} // namespace lemlib