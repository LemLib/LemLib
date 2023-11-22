#include "lemlib/util.hpp"
#include "lemlib/timer.hpp"
#include "lemlib/logger/logger.hpp"
#include "lemlib/odom/differentialArc.hpp"

namespace lemlib {
/**
 * Construct a odometry through tracking arcs
 *
 * This can use vertical tracking wheels, horizontal tracking wheels, and imus.
 * Not all of them are needed however. For example, if you have 2 parallel tracking
 * wheels, you don't need an imu. If you have good traction wheels, you also dont need
 * any horizontal wheels.
 *
 * Vectors are passed since they can have a varying number of sensors in them, allowing for
 * any tracking wheel + imu setup
 */
DifferentialArc::DifferentialArc(std::vector<TrackingWheel>& verticals, std::vector<TrackingWheel>& horizontals,
                                 std::vector<TrackingWheel>& drivetrain, std::vector<std::shared_ptr<Gyro>>& gyros)
    : verticals(verticals),
      horizontals(horizontals),
      drivetrain(drivetrain),
      gyros(gyros) {}

/**
 * Calibrate the sensors
 *
 * We have to calibrate tracking wheels and imus. We calibrate them all and remove any that fail
 * calibration. The encoders will output errors if they fail to calibrate.
 */
void DifferentialArc::calibrate(bool calibrateGyros) {
    std::vector<TrackingWheel> newVerticals = {};
    std::vector<TrackingWheel> newHorizontals = {};
    std::vector<TrackingWheel> newDrivetrain = {};
    std::vector<std::shared_ptr<Gyro>> newGyros = {};

    // calibrate vertical tracking wheels
    for (auto it = verticals.begin(); it != verticals.end(); it++) {
        if (it->reset()) {
            infoSink()->warn("Vertical tracker at offset {} failed calibration!", it->getOffset().convert(in));
        } else newVerticals.push_back(*it);
    }

    // calibrate horizontal tracking wheels
    for (auto it = horizontals.begin(); it != horizontals.end(); it++) {
        if (it->reset()) {
            infoSink()->warn("Horizontal tracker at offset {} failed calibration!", it->getOffset().convert(in));
        } else newHorizontals.push_back(*it);
    }

    // calibrate drivetrain motors
    for (auto it = drivetrain.begin(); it != drivetrain.end(); it++) {
        if (it->reset()) {
            if (units::sgn(it->getOffset()) == 1) infoSink()->warn("Left drivetrain motor failed to calibrate!");
            else infoSink()->warn("Right drivetrain motor failed to calibrate!");
        } else newDrivetrain.push_back(*it);
    }

    if (!calibrateGyros) return; // return if we don't need to calibrate gyros
    // calibrate gyros
    for (auto& it : gyros) it->calibrate();
    Timer timer(3_sec); // try calibrating gyros for 3 seconds
    while (!timer.isDone()) {
        for (auto& gyro : gyros) { // continuously calibrate in case of failure
            if (!gyro->isCalibrating() && !gyro->isCalibrated()) gyro->calibrate();
        }
        pros::delay(10);
    }

    for (auto it = gyros.begin(); it != gyros.end(); it++) {
        if (!(**it).isCalibrated()) {
            infoSink()->warn("IMU on port {} failed to calibrate! Removing...", (**it).getPort());
        } else {
            newGyros.push_back(*it);
        }
    }

    verticals = newVerticals;
    horizontals = newHorizontals;
    drivetrain = newDrivetrain;
    gyros = newGyros;
}

/**
 * @brief Calculate the change in heading given 2 tracking wheels
 *
 * @note positive change in counterclockwise
 *
 * @param tracker1 the first tracking wheel
 * @param tracker2 the second tracking wheel
 * @return change in angle
 */
Angle calcDeltaTheta(TrackingWheel& tracker1, TrackingWheel& tracker2) {
    const Length numerator = tracker1.getDistanceDelta(false) - tracker2.getDistanceDelta(false);
    const Length denominator = tracker1.getOffset() - tracker2.getOffset();
    return rad * numerator / denominator;
}

/**
 * @brief Calculate the change in heading given a vector of imus
 *
 * @note positive change in counterclockwise
 *
 * @param gyros vector of Gyro shared pointers
 * @return Angle the average change in heading
 */
Angle calcDeltaTheta(std::vector<std::shared_ptr<Gyro>>& gyros) {
    Angle deltaTheta = 0_rad;
    for (const auto& gyro : gyros) deltaTheta += gyro->getRotationDelta() / gyros.size();
    return deltaTheta;
}

/**
 * Update Arc Odom
 *
 * Tracking through arcs works through estimating the robot's change in position between
 * angles as an arc, rather than a straight line, improving accuracy.
 *
 * This alg can either use tracking wheels to calculate angle, or preferably an imu, or motor encoders
 * as a last resort.
 * Theoretically you can get better performance with tracking wheels, but this is very
 * difficult to achieve.
 *
 * 5225A has published a fantastic paper on this odom algorithm:
 * http://thepilons.ca/wp-content/uploads/2018/10/Tracking.pdf
 */
void DifferentialArc::update() {
    // calculate theta
    // Priority:
    // 1. IMU
    // 2. Horizontal tracking wheels
    // 3. Vertical tracking wheels
    // 4. Drivetrain motor encoders
    Angle theta = pose.theta;
    if (gyros.size() > 0) { // calculate heading with imus if we have enough
        theta += calcDeltaTheta(gyros);
    } else if (horizontals.size() > 1) { // calculate heading with horizontal tracking wheels if we have enough
        theta += calcDeltaTheta(horizontals.at(0), horizontals.at(1));
    } else if (verticals.size() > 1) { // calculate heading with vertical tracking wheels if we have enough
        theta += calcDeltaTheta(verticals.at(0), verticals.at(1));
    } else if (drivetrain.size() > 1) { // calculate heading with drivetrain motor encoders
        theta += calcDeltaTheta(drivetrain.at(0), drivetrain.at(1));
    } else {
        infoSink()->error("Odom calculation failure! Not enough sensors to calculate heading");
        return;
    }
    const Angle deltaTheta = theta - pose.theta;
    const Angle avgTheta = pose.theta + deltaTheta / 2;

    // calculate local change in position
    Pose local(0_m, 0_m, deltaTheta);
    // set sinDTheta2 to 1 if deltaTheta is 0. Simplifies local position calculations.
    const float sinDTheta2 = (deltaTheta == 0_rad) ? 1 : 2 * units::sin(deltaTheta / 2).raw();

    // calculate local y position
    for (auto& tracker : horizontals) {
        // prevent divide by 0
        const Length radius = (deltaTheta == 0_rad) ? tracker.getDistanceDelta()
                                                    : tracker.getDistanceDelta() / deltaTheta.convert(rad) +
                                                          tracker.getOffset(); // todo test
        local.y += sinDTheta2 * radius / horizontals.size();
    }

    // calculate local x position
    if (verticals.size() > 0) { // use dedicated tracking wheels if we have any
        for (auto& tracker : verticals) {
            const Length radius = (deltaTheta == 0_rad)
                                      ? tracker.getDistanceDelta()
                                      : tracker.getDistanceDelta() / deltaTheta.convert(rad) + tracker.getOffset();
            local.x += sinDTheta2 * radius / verticals.size();
        }
    } else if (drivetrain.size() > 0) { // use motor encoders if we have no dedicated tracking wheels
        for (auto& motor : drivetrain) {
            const Length radius = (deltaTheta == 0_rad)
                                      ? motor.getDistanceDelta()
                                      : motor.getDistanceDelta() / deltaTheta.convert(rad) + motor.getOffset();
            local.x += sinDTheta2 * radius / drivetrain.size();
        }
    } else { // output a warning if there are no available sensors to calculate local x
        infoSink()->warn("No vertical tracking wheels! Assuming y movement is 0");
    }

    // calculate global position
    pose += local.rotate(avgTheta);
}
}; // namespace lemlib