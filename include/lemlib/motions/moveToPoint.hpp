#pragma once

#include "hardware/Motor/MotorGroup.hpp"
#include "lemlib/ExitCondition.hpp"
#include "lemlib/PID.hpp"
#include "units/Pose.hpp"
#include <functional>


namespace lemlib {

struct MoveToPointParams {
    bool reversed = false;
    Number maxLateralSpeed = 1;
    Number minLateralSpeed = 0;
    Number maxAngularSpeed = 1;
    Number lateralSlew = INFINITY;
    Number angularSlew = INFINITY;
    Length earlyExitRange = 0_in;
};

struct MoveToPointSettings {
    PID angularPID;
    PID lateralPID;
    ExitConditionGroup<Length> exitConditions;
    std::function<units::Pose()> poseGetter;
    lemlib::MotorGroup& leftMotors;
    lemlib::MotorGroup& rightMotors;
};

void moveToPoint(units::V2Position target,
                 Time timeout,
                 MoveToPointParams params,
                 MoveToPointSettings settings);

}; // namespace lemlib
