#include "lemlib/motions/motion.hpp"
#include "lemlib/chassis/flags.hpp"
#include <optional>

namespace lemlib {

/**
 * @brief Parameters for Chassis::Turn
 *
 * We use a struct to simplify customization. Chassis::Turn has many
 * parameters and specifying them all just to set one optional param ruins
 * readability. By passing a struct to the function, we can have named
 * parameters, overcoming the c/c++ limitation
 */
struct TurnToParams {
        /** whether the robot should turn to face the point with the front of the robot. True by default */
        bool forwards = true;
        /** the direction the robot should turn in. AUTO by default */
        AngularDirection direction = AngularDirection::AUTO;
        /** the maximum speed the robot can turn at. Value between 0-127. 127 by default */
        int maxSpeed = 127;
        /** the minimum speed the robot can turn at. If set to a non-zero value, the `it conditions will switch to less
         * accurate but smoother ones. Value between 0-127. 0 by default */
        int minSpeed = 0;
        /** angle between the robot and target point where the movement will exit. Only has an effect if minSpeed is
         * non-zero.*/
        float earlyExitRange = 0;
        float slew = 0;
        std::vector<ExitCondition>& exits;
};

/**
 * @brief Turn class. Derived from Movement
 */
class DifferentialTurn : public DifferentialMotion {
    public:
        /**
         * @brief Construct a new Turn movement
         *
         * @param angularPID the angular PID to use
         * @param target the target heading. Degrees, 0 is right, increases counterclockwise
         * @param params parameters for the motion
         */
        DifferentialTurn(PID& angularPID, float target, int timeout, TurnToParams& params);

        /**
         * @brief Construct a new Turn movement
         *
         * @param angularPID the angular PID to use
         * @param target the target pose
         * @param params parameters for the motion
         */
        DifferentialTurn(PID& angularPID, Pose target, int timeout, TurnToParams& params);

        /**
         * @brief Update the movement
         *
         * @note This function should be called in a loop
         * @note Units are in inches and radians. Angle 0 is right, and increases counterclockwise
         *
         * @param pose the position of the chassis
         *
         * @return std::pair<int, int> left and right motor power respectively. 128 means movement is done
         */
        DifferentialChassisSpeeds calculate(Pose pose) override;
    private:
        float distTraveled = 0;
        PID& angularPID;
        float startTheta = 0;
        std::optional<float> targetHeading = std::nullopt;
        std::optional<Pose> targetPose = std::nullopt;
        Pose startPose = Pose(0, 0, 0);
        TurnToParams params;
        Timer timer;
        bool settling;
        std::optional<float> prevDeltaTheta;
        std::optional<float> prevRawDeltaTheta;
        float prevMotorPower = 0;

        int compState;
        int state = -1; // -1 = not started, 0 = in progress, 1 = done
};
} // namespace lemlib