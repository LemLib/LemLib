#include "lemlib/pid.hpp"
#include "pros/motor_group.hpp"
#include "pros/motors.hpp"

namespace lemlib {

enum class PIDBeingTuned {
   distance = 0,
   turning = 1,
   flywheel = 2, 
   catapult = 3,
   misc = 4
};
    
class PIDTuner {
private:

    Gains gains;
    FAPID* pid;

    std::vector<pros::MotorGroup> motorGroups;

    void tuneFlywheel(int targetRPM, float gearRatio);
    void tuneMisc();

public:

    /**
     * @brief Construct a new PIDTuner object
     * 
     * @param pidArg The PID controller to tune the constants in
     * @param motorGroupArg The motor group that is being tuned
     * @param tunekF Whether to tune the Feed Forward Constant (kF)
     * @param tunekA Whether to tune the Acceleration Gain Constant (kA)
     * @param tunekP Whether to tune the Proportional Gain Constant (kP)
     * @param tunekI Whether to tune the Integral Gain Constant (kI)
     * @param tunekD Whether to tune the Derivative Gain Constant (kD)
     */
    PIDTuner(FAPID* pidArg, pros::MotorGroup motorGroupArg, bool tunekF, bool tunekA, bool tunekP, bool tunekI, bool tunekD);

    /**
     * @brief Construct a new PIDTuner object
     * 
     * @param pidArg The PID controller to tune the constants in
     * @param motorGroupArg The motor group that is being tuned
     * @param tunekF Whether to tune the Feed Forward Constant (kF)
     * @param tunekA Whether to tune the Acceleration Gain Constant (kA)
     * @param tunekP Whether to tune the Proportional Gain Constant (kP)
     * @param tunekI Whether to tune the Integral Gain Constant (kI)
     * @param tunekD Whether to tune the Derivative Gain Constant (kD)
     */
    PIDTuner(FAPID* pidArg, std::vector<pros::MotorGroup> motorGroupArg, bool tunekF, bool tunekA, bool tunekP, bool tunekI, bool tunekD);

    ~PIDTuner();

    void tunePID();

};

}