#include "lemlib/motions/moveToPose.hpp"
#include "lemlib/MotionCancelHelper.hpp"
#include "LemLog/logger/Helper.hpp"
#include "lemlib/Timer.hpp"
#include "units/Angle.hpp"
#include "lemlib/util.hpp"
#include <cmath>

static logger::Helper logHelper("lemlib/motions/moveToPose");

void lemlib::moveToPose(Length x, Length y, AngleRange theta, Time timeout, lemlib::MoveToPoseParams params,
                        lemlib::MoveToPoseSettings settings) {
    logHelper.info("Moving to {} in, {} in, {} cDeg", x.convert(in), y.convert(in), to_cDeg(theta));

    // sanitize inputs
    params.minSpeed = fabs(params.minSpeed);
    // reset PIDs
    settings.lateralExitConditions.reset();
    settings.angularExitConditions.reset();

    settings.lateralPID.reset();
    settings.angularPID.reset();
    // initialize persistent variables
    double previousLateralOut = 0.0;
    double previousAngularOut = 0.0;

    lemlib::Timer timer(timeout);
    bool previouslySameSide = false;
    bool close = false;

    AngleRange angularError = 0_stDeg;
    Length lateralError = 0_in;

    // create the target pose
    units::Pose target(x, y, theta);
    // if we are moving backwards, we need to flip the orientation
    if (!params.forwards) target.orientation = units::mod(target.orientation + 180_stDeg, 360_stDeg);

    lemlib::MotionCancelHelper helper(10_msec); // cancel helper
    // loop until the motion has been cancelled, the timer is done, or an exit condition has been met
    while (helper.wait() && !timer.isDone() && !settings.lateralExitConditions.update(lateralError) &&
           !settings.angularExitConditions.update(angularError)) {
        // get the position of the robot
        const units::Pose pose = settings.poseGetter();

        // calculate the distance to the target
        const Length distanceToTarget = pose.distanceTo(target);

        // check if the robot is close enough to begin settling
        if (distanceToTarget < 7.5_in && close == false) {
            close = true;
            params.maxSpeed = fmax(fabs(previousLateralOut), params.maxSpeed);
        }

        // calculate the carrot point
        units::Pose carrot = close ? target
                                   : target - (units::Pose(units::cos(target.orientation) * in,
                                                           units::sin(target.orientation) * in, 0_stDeg) *
                                               params.lead * distanceToTarget.convert(in));

        // check if the robot is on the same side of the target as the carrot
        const bool robotSide = (pose.y - target.y) * -units::sin(target.orientation) <=
                               (pose.x - target.x) * units::cos(target.orientation) + params.earlyExitRange;
        const bool carrotSide = (carrot.y - target.y) * -units::sin(target.orientation) <=
                                (carrot.x - target.x) * units::cos(target.orientation) + params.earlyExitRange;
        const bool sameSide = robotSide == carrotSide;

        // exit the motion to immediately continue to the next one if the robot is on the same side of the target as the
        // carrot
        if (!sameSide && previouslySameSide && close && params.minSpeed != 0) break;
        previouslySameSide = sameSide;

        const Angle adjustedTheta = params.forwards ? pose.orientation : pose.orientation + 180_stDeg;
        const Angle angularError =
            close ? angleError(adjustedTheta, target.orientation) : angleError(adjustedTheta, pose.angleTo(carrot));

        Length lateralError = pose.distanceTo(carrot);
        // only use cos when settling
        // otherwise just multiply by the sin of cos
        // maxSlipSpeed takes care of lateralOut
        if (close) lateralError *= units::cos(angleError(pose.orientation, pose.angleTo(carrot))).internal();
        else lateralError *= units::sgn(units::cos(angleError(pose.orientation, pose.angleTo(carrot))));

        // get output from PIDs
        double lateralOut = settings.lateralPID.update(lateralError.convert(in));
        double angularOut = settings.angularPID.update(angularError.convert(deg));

        // restrict speeds
        lateralOut = std::clamp(lateralOut, -params.maxSpeed, params.maxSpeed);
        angularOut = std::clamp(angularOut, -params.maxSpeed, params.maxSpeed);

        if (!close) lateralOut = lemlib::slew(lateralOut, previousLateralOut, settings.lateralPID.getGains().slew);

        // constrain lateral output by max speed it can travel at without slipping
        const double radius = 1 / fabs(lemlib::getCurvature(pose, carrot));
        const double maxSlipSpeed = sqrt(params.horizontalDrift * radius * 9.8);
        lateralOut = std::clamp(lateralOut, -maxSlipSpeed, maxSlipSpeed);
        // prioritize angular output over lateral movement
        const double overturn = fabs(angularOut) + fabs(lateralOut) - params.maxSpeed;
        if (overturn > 0) lateralOut -= lateralOut > 0 ? overturn : -overturn;

        // prevent moving in the wrong direction
        if (params.forwards && !close) lateralOut = std::fmax(lateralOut, 0);
        else if (!params.forwards && !close) lateralOut = std::fmin(lateralOut, 0);

        // constrain lateral output by minimum speed
        if (params.forwards && lateralOut < fabs(params.minSpeed) && lateralOut > 0) lateralOut = fabs(params.minSpeed);
        if (!params.forwards && -lateralOut < fabs(params.minSpeed) && lateralOut < 0)
            lateralOut = -fabs(params.minSpeed);

        // update previous output
        previousLateralOut = lateralOut;
        previousAngularOut = angularOut;

        // ratio the speeds to respect the max
        std::tuple<double, double> power = lemlib::ratioSpeeds(lateralOut, angularOut, params.maxSpeed);

        logHelper.debug("Moving with {} power, {} power, error: {} in, {} cDeg", std::get<0>(power), std::get<1>(power),
                        lateralError.convert(in), to_cDeg(angularError));

        // move the motors
        settings.leftMotors.move(std::get<0>(power));
        settings.rightMotors.move(std::get<1>(power));
    }

    settings.leftMotors.move(0);
    settings.rightMotors.move(0);

    logHelper.info("Finished moving to {} in, {} in, {} cDeg, current pose {} in, {} in, {} cDeg", x.convert(in),
                   y.convert(in), to_cDeg(theta), settings.poseGetter().x.convert(in),
                   settings.poseGetter().y.convert(in), to_cDeg(settings.poseGetter().orientation));
}
