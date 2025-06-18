#include <cmath>
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/logger/logger.hpp"
#include "lemlib/timer.hpp"
#include "lemlib/util.hpp"
#include "pros/misc.hpp"

void lemlib::Chassis::turnToPoint(float x, float y, int timeout, TurnToPointParams params, bool async) {
    /**
     * Mutex that is used to block until the motion has started.
     */
    pros::Mutex startedMutex;
    pros::Task task([&]() {
        startedMutex.take();
        this->requestMotionStart();
        startedMutex.give();
        // were all motions cancelled?
        if (!this->motionRunning) return;

        params.minSpeed = std::abs(params.minSpeed);

        float targetTheta;
        float deltaX, deltaY, deltaTheta;
        float motorPower;
        float prevMotorPower = 0;
        float startTheta = getPose().theta;
        bool settling = false;
        std::optional<float> prevRawDeltaTheta = std::nullopt;
        std::optional<float> prevDeltaTheta = std::nullopt;
        std::uint8_t compState = pros::competition::get_status();
        distTraveled = 0;
        Timer timer(timeout);
        angularLargeExit.reset();
        angularSmallExit.reset();
        angularPID.reset();

        // main loop
        while (!timer.isDone() && !angularLargeExit.getExit() && !angularSmallExit.getExit() && this->motionRunning) {
            // update variables
            Pose pose = getPose();
            pose.theta = (params.forwards) ? fmod(pose.theta, 360) : fmod(pose.theta - 180, 360);

            // update completion vars
            distTraveled = fabs(angleError(pose.theta, startTheta, false));

            deltaX = x - pose.x;
            deltaY = y - pose.y;
            targetTheta = fmod(radToDeg(M_PI_2 - atan2(deltaY, deltaX)), 360);

            // check if settling
            const float rawDeltaTheta = angleError(targetTheta, pose.theta, false);
            if (prevRawDeltaTheta == std::nullopt) prevRawDeltaTheta = rawDeltaTheta;
            if (sgn(rawDeltaTheta) != sgn(prevRawDeltaTheta)) settling = true;
            prevRawDeltaTheta = rawDeltaTheta;

            // calculate deltaTheta
            if (settling) deltaTheta = angleError(targetTheta, pose.theta, false);
            else deltaTheta = angleError(targetTheta, pose.theta, false, params.direction);
            if (prevDeltaTheta == std::nullopt) prevDeltaTheta = deltaTheta;

            // motion chaining
            if (params.minSpeed != 0 && fabs(deltaTheta) < params.earlyExitRange) break;
            if (params.minSpeed != 0 && sgn(deltaTheta) != sgn(prevDeltaTheta)) break;

            // calculate the speed
            motorPower = angularPID.update(deltaTheta);
            angularLargeExit.update(deltaTheta);
            angularSmallExit.update(deltaTheta);

            // cap the speed
            if (motorPower > params.maxSpeed) motorPower = params.maxSpeed;
            else if (motorPower < -params.maxSpeed) motorPower = -params.maxSpeed;
            if (fabs(deltaTheta) > 20) motorPower = slew(motorPower, prevMotorPower, angularSettings.slew);
            if (motorPower < 0 && motorPower > -params.minSpeed) motorPower = -params.minSpeed;
            else if (motorPower > 0 && motorPower < params.minSpeed) motorPower = params.minSpeed;
            prevMotorPower = motorPower;

            infoSink()->debug("Turn Motor Power: {} ", motorPower);

            // move the drivetrain
            drivetrain.leftMotors->move(motorPower);
            drivetrain.rightMotors->move(-motorPower);

            pros::delay(10);
        }

        // stop the drivetrain
        drivetrain.leftMotors->move(0);
        drivetrain.rightMotors->move(0);
        // set distTraveled to -1 to indicate that the function has finished
        distTraveled = -1;
        this->endMotion();
    });
    pros::delay(10);
    startedMutex.take(TIMEOUT_MAX);
    if (!async) {
        do pros::delay(10);
        while (!(task.get_state() == pros::E_TASK_STATE_INVALID || task.get_state() == pros::E_TASK_STATE_SUSPENDED || task.get_state() == pros::E_TASK_STATE_DELETED));
    }
}
