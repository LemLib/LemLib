#pragma once

#include "hardware/Motor/MotorGroup.hpp"
#include "units/Pose.hpp"
#include "lemlib/ExitCondition.hpp"
#include "lemlib/PID.hpp"
#include <functional>

namespace lemlib {

struct MoveToPoseParams {
        bool reversed = false;
        Number driftCompensation = 2; // TODO: figure out how we're going to deal with a user defined default
        Number lead = 0.6;
        Number maxLateralSpeed = 1;
        Number minLateralSpeed = 0;
        Number maxAngularSpeed = 1;
        Number lateralSlew = INFINITY;
        Number angularSlew = INFINITY;
        Length earlyExitRange = 0_in;
};

struct MoveToPoseSettings {
        PID angularPID;
        PID lateralPID;
        ExitConditionGroup<Length> lateralExitConditions;
        ExitConditionGroup<AngleRange> angularExitConditions;
        std::function<units::Pose()> poseGetter;
        lemlib::MotorGroup& leftMotors;
        lemlib::MotorGroup& rightMotors;
};

void moveToPose(units::Pose target, Time timeout, MoveToPoseParams params, MoveToPoseSettings settings);

}; // namespace lemlib