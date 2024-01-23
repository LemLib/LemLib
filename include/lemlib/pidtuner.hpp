#include "lemlib/pid.hpp"
#include "pros/motor_group.hpp"
#include "pros/motors.hpp"
#include <vector>

namespace lemlib {

enum class TUNER_TARGET { distance = 0, turning = 1, flywheel = 2, catapult = 3, misc = 4 };

class PIDTuner {
    private:
        Gains gains;
        std::shared_ptr<FAPID> pid;

        TUNER_TARGET target;

        std::shared_ptr<std::vector<pros::MotorGroup>> motorGroups;

        void tuneFlywheel(int targetRPM, float gearRatio);
        void tuneCatapult(int targetRPM);
        void tuneDistance(float targetDistance);
        void tuneTurning(float targetAngle);
        void tuneMisc(float target);
    public:
        /**
         * @brief Construct a new PIDTuner object
         *
         * @param pidArg The PID controller to tune the constants in
         * @param motorGroupArg The motor group that is being tuned
         * @param target The Target System that is being tuned
         * @param tunekF Whether to tune the Feed Forward Constant (kF)
         * @param tunekA Whether to tune the Acceleration Gain Constant (kA)
         * @param tunekP Whether to tune the Proportional Gain Constant (kP)
         * @param tunekI Whether to tune the Integral Gain Constant (kI)
         * @param tunekD Whether to tune the Derivative Gain Constant (kD)
         */
        PIDTuner(std::shared_ptr<FAPID>& pid, std::shared_ptr<std::vector<pros::MotorGroup>>& motorGroups,
                 TUNER_TARGET target, bool tunekF, bool tunekA, bool tunekP, bool tunekI, bool tunekD);

        ~PIDTuner();

        bool tunePID();
};

} // namespace lemlib