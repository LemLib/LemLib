#pragma once

#include "lemlib/config.hpp"
#include <functional>

namespace lemlib {

struct MoveToPoseParams {
        bool reversed = false;
        Number driftCompensation = drift_compensation;
        Number lead = 0.6;
        Number maxLateralSpeed = 1;
        Number minLateralSpeed = 0;
        Number maxAngularSpeed = 1;
        Number lateralSlew = lateral_slew;
        Number angularSlew = angular_slew;
        Length earlyExitRange = 0_in;
};

struct MoveToPoseSettings {
        PID angularPID = angular_pid;
        PID lateralPID = lateral_pid;
        ExitConditionGroup<Length> lateralExitConditions = lateral_exit_conditions;
        ExitConditionGroup<AngleRange> angularExitConditions = angular_exit_conditions;
        std::function<units::Pose()> poseGetter = pose_getter;
        lemlib::MotorGroup& leftMotors = left_motors;
        lemlib::MotorGroup& rightMotors = right_motors;
};

void moveToPose(units::Pose target, Time timeout, MoveToPoseParams params, MoveToPoseSettings settings);

}; // namespace lemlib