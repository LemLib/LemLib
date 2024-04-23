#include "lemlib/util.hpp"
#include <cmath>

namespace lemlib {
ExpoDriveCurve::ExpoDriveCurve(float deadband, float minOutput, float curve)
    : deadband(deadband),
      minOutput(minOutput),
      curveGain(curve) {}

float ExpoDriveCurve::curve(float input) {
    // return 0 if input is within deadzone
    if (fabs(input) <= deadband) return 0;
    // g is the output of g(x) as defined in the Desmos graph
    const float g = fabs(input) - deadband;
    // g127 is the output of g(127) as defined in the Desmos graph
    const float g127 = 127 - deadband;
    // i is the output of i(x) as defined in the Desmos graph
    const float i = pow(curveGain, g - 127) * g * sgn(input);
    // i127 is the output of i(127) as defined in the Desmos graph
    const float i127 = pow(curveGain, g127 - 127) * g127;
    return (127.0 - minOutput) / (127) * i * 127 / i127 + minOutput * sgn(input);
}
} // namespace lemlib