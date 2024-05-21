#include "lemlib/pose.hpp"

namespace lemlib {
enum class MotionState { RUNNING, FINISHED};

struct DifferentialChassisSpeeds {
        float L, R;
};

struct HolonomicChassisSpeeds {
        float X, Y, Omega;
};

class DifferentialMotion {
    public:
        virtual DifferentialChassisSpeeds calculate(Pose pose) = 0;
        virtual MotionState getState() = 0;
        virtual ~DifferentialMotion();
};

class HolonomicMotion {
    public:
        virtual HolonomicChassisSpeeds calculate(Pose pose) = 0;
        virtual MotionState getState() = 0;
        virtual ~HolonomicMotion();
};

} // namespace lemlib