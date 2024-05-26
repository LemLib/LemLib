#include "units/units.hpp"
#include "units/Vector2D.hpp"

namespace lemlib {
/**
 * @enum MotionState
 * 
 * @brief Enum to represent the state of a motion
 */
enum class MotionState {
    RUNNING, /** the motion is currently running */
    FINISHED /** the motion is finished */
};

/**
 * @struct DifferentialChassisSpeeds1
 * 
 * @brief represents the speed of the left and right wheels of a differential drive robot
 */
struct DifferentialChassisSpeeds {
        Voltage left; /** the speed of the left wheels */
        Voltage right; /** the speed of the right wheels */
};

/**
 * @struct HolonomicChassisSpeeds
 * 
 * @brief represents the speed of the x, y, and angular velocity of a holonomic drive robot
 */
struct HolonomicChassisSpeeds {
        LinearVelocity x; /** x velocity */
        LinearVelocity y; /** y velocity */
        AngularVelocity omega; /** angular velocity */
};

/**
 * @class DifferentialMotion
 * 
 * @brief Abstract class which represents a motion algorithm for a differential drive robot
 *
 * We use this abstraction to allow for different motion algorithms to be used with the same interface.
 * This allows us to easily create and maintain motion algorithms while also allowing the user to
 * make custom motion algorithms without having to recompile the library.
 */
class DifferentialMotion {
    public:
        /**
         * @brief Calculates the speed of the left and right wheels of a differential drive robot
         * 
         * @param pose the current pose of the robot
         * @return DifferentialChassisSpeeds the speed of the left and right wheels
         */
        virtual DifferentialChassisSpeeds calculate(units::Pose pose) = 0;
        /**
         * @brief Get the state of the motion
         * 
         * @return MotionState the state of the motion
         */
        MotionState getState();
        /**
         * @brief Destroy the Differential Motion object
         * 
         */
        virtual ~DifferentialMotion();
    protected:
        MotionState state = MotionState::RUNNING; /** the state of the motion */
};

/**
 * @class HolonomicMotion
 * 
 * @brief Abstract class which represents a motion algorithm for a holonomic drive robot
 *
 * We use this abstraction to allow for different motion algorithms to be used with the same interface.
 * This allows us to easily create and maintain motion algorithms while also allowing the user to
 * make custom motion algorithms without having to recompile the library.
 */
class HolonomicMotion {
    public:
        /**
         * @brief Calculates the field-relative speed of a holonomic drive robot
         * 
         * @param pose the current pose of the robot
         * @return HolonomicChassisSpeeds the field-relative speed of the robot
         */
        virtual HolonomicChassisSpeeds calculate(units::Pose pose) = 0;
        /**
         * @brief Get the state of the motion
         * 
         * @return MotionState the state of the motion
         */
        MotionState getState();
        /**
         * @brief Destroy the Holonomic Motion object
         * 
         */
        virtual ~HolonomicMotion();
    protected:
        MotionState state = MotionState::RUNNING; /** the state of the motion */
};

} // namespace lemlib