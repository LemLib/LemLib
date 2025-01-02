#pragma once

#include "lemlib/ExitCondition.hpp"
#include "lemlib/util.hpp"
#include "pros/motor_group.hpp"
#include "units/Angle.hpp"
#include "units/units.hpp"
#include "units/Pose.hpp"
#include "lemlib/pid.hpp"
#include <functional>

namespace lemlib {
struct TurnToHeadingParams {
        AngularDirection direction = AngularDirection::AUTO;
        int maxSpeed = 127;
        int minSpeed = 0;
        Angle earlyExitRange = 0_cRot;
};

struct TurnToHeadingSettings {
        PID angularPID;
        ExitCondition angularLargeExit;
        ExitCondition angularSmallExit;
        std::function<units::Pose()> poseGetter;
        std::shared_ptr<pros::MotorGroup> leftMotors;
        std::shared_ptr<pros::MotorGroup> rightMotors;
};

void turnToHeading(Angle heading, Time timeout, TurnToHeadingParams params, TurnToHeadingSettings settings);
} // namespace lemlib