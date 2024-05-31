#include "lemlib/motions/motion.hpp"
#include "units/units.hpp"

namespace lemlib {
DifferentialMotion::DifferentialMotion(const Length trackWidth, const LinearVelocity maxDriveVelocity,
                                       const float desaturateBias)
    : trackWidth(trackWidth),
      maxDriveVelocity(maxDriveVelocity),
      desaturateBias(desaturateBias) {}

bool DifferentialMotion::isRunning() const { return running; }

DifferentialChassisSpeeds DifferentialMotion::desaturate(DifferentialChassisSpeeds speeds) const {
    // get throttle and turn velocity
    LinearVelocity throttle = speeds.linearVelocity;
    LinearVelocity turn = (speeds.angularVelocity * (trackWidth / 2)).val();
    // desaturate if necessary
    if (units::abs(throttle) + units::abs(turn) > maxDriveVelocity) {
        const LinearVelocity prevThrottle = throttle;
        throttle *= (1 - desaturateBias * std::abs(turn.val() / maxDriveVelocity.val()));
        turn *= (1 - (1 - desaturateBias) * std::abs(prevThrottle.val() / maxDriveVelocity.val()));
    }
    // return desaturated speeds
    return {throttle, turn.val() / (trackWidth / 2).val()};
}

DifferentialMotion::~DifferentialMotion() {}

bool HolonomicMotion::isRunning() const { return running; }

HolonomicMotion::~HolonomicMotion() {}
} // namespace lemlib