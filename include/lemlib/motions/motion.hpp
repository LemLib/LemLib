#include "lemlib/pose.hpp"

namespace lemlib {
enum OutputType { VOLTAGE, VELOCITY };

struct DifferentialChassisSpeeds {
        float L, R;
        OutputType outType = VOLTAGE;
};

struct HolonomicChassisSpeeds {
        float X, Y, Omega;
        OutputType outType = VOLTAGE;
};

class DifferentialMotion {
    public:
        virtual DifferentialChassisSpeeds calculate(Pose pose) = 0;
        virtual ~DifferentialMotion();
};

class HolonomicMotion {
    public:
        virtual HolonomicChassisSpeeds calculate(Pose pose) = 0;
        virtual ~HolonomicMotion();
};

} // namespace lemlib