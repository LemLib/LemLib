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
        MotionState getState();
        virtual ~DifferentialMotion();
    protected:
        MotionState state = MotionState::RUNNING;
};

class HolonomicMotion {
    public:
        virtual HolonomicChassisSpeeds calculate(Pose pose) = 0;
        MotionState getState();
        virtual ~HolonomicMotion();
    protected:
        MotionState state = MotionState::RUNNING;
};

} // namespace lemlib