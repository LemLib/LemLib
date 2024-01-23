#include "lemlib/util.hpp"
#include "lemlib/timer.hpp"
#include "lemlib/logger/logger.hpp"
#include "lemlib/odom/differentialArc.hpp"
#include <memory>

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
DifferentialArc::DifferentialArc(std::vector<std::shared_ptr<TrackingWheel>>& verticals,
                                 std::vector<std::shared_ptr<TrackingWheel>>& horizontals,
                                 std::vector<std::shared_ptr<TrackingWheel>>& drivetrain,
                                 std::vector<std::shared_ptr<pros::GPS>>& gps,
                                 std::vector<std::shared_ptr<pros::IMU>>& imus,
                                 std::vector<std::shared_ptr<Gyro>>& gyros)
    : verticals(verticals),
      horizontals(horizontals),
      drivetrain(drivetrain),
      gps(gps),
      imus(imus),
      gyros(gyros) {}

/**
 * Calibrate the sensors
 *
 * We have to calibrate tracking wheels and imus. We calibrate them all and remove any that fail
 * calibration. The encoders will output errors if they fail to calibrate.
 */
void DifferentialArc::calibrate(bool calibrateGyros) {
    std::vector<std::shared_ptr<TrackingWheel>> newVerticals = {};
    std::vector<std::shared_ptr<TrackingWheel>> newHorizontals = {};
    std::vector<std::shared_ptr<TrackingWheel>> newDrivetrain = {};
    std::vector<std::shared_ptr<Gyro>> newGyros = {};
    std::vector<std::shared_ptr<pros::GPS>> gps = {};
    std::vector<std::shared_ptr<pros::IMU>> imus = {};

    // calibrate vertical tracking wheels
    for (auto it = this->verticals.begin(); it != this->verticals.end(); it++) {
        if (it->get()->reset()) {
            infoSink()->warn("Vertical tracker at offset {} failed calibration!", it->get()->getOffset());
        } else newVerticals.push_back(*it);
    }

    // calibrate horizontal tracking wheels
    for (auto it = this->horizontals.begin(); it != this->horizontals.end(); it++) {
        if (it->get()->reset()) {
            infoSink()->warn("Horizontal tracker at offset {} failed calibration!", it->get()->getOffset());
        } else newHorizontals.push_back(*it);
    }

    // calibrate drivetrain motors
    for (auto it = this->drivetrain.begin(); it != this->drivetrain.end(); it++) {
        if (it->get()->reset()) {
            if (sgn(it->get()->getOffset() == 1)) infoSink()->warn("Left drivetrain motor failed to calibrate!");
            else infoSink()->warn("Right drivetrain motor failed to calibrate!");
        } else newDrivetrain.push_back(*it);
    }

    if (!calibrateGyros) return; // return if we don't need to calibrate gyros
    // calibrate gyros
    for (auto& it : this->gyros) it->calibrate();
    Timer gyrotimer(3000); // try calibrating gyros for 3000 ms
    while (!gyrotimer.isDone()) {
        for (auto& gyro : this->gyros) { // continuously calibrate in case of failure
            if (!gyro->isCalibrating() && !gyro->isCalibrated()) gyro->calibrate();
        }
        pros::delay(10);
    }

    for (auto it = this->gyros.begin(); it != this->gyros.end(); it++) {
        if (!(**it).isCalibrated()) {
            infoSink()->warn("IMU on port {} failed to calibrate! Removing...", (**it).getPort());
        } else {
            newGyros.push_back(*it);
        }
    }

    // calibrate imus
    for (auto& it : this->imus) it.reset();
    Timer imutimer(3000); // try calibrating gyros for 3000 ms
    while (!imutimer.isDone()) {
        for (std::shared_ptr<pros::IMU>& imu : this->imus) { // continuously calibrate in case of failure
            if (!imu->is_calibrating()) imu.reset();
        }
        pros::delay(10);
    }

    this->verticals = newVerticals;
    this->horizontals = newHorizontals;
    this->drivetrain = newDrivetrain;
    this->gyros = newGyros;
}

/**
 * @brief Calculate the change in heading given 2 tracking wheels
 *
 * @note positive change in counterclockwise
 *
 * @param tracker1 the first tracking wheel
 * @param tracker2 the second tracking wheel
 * @return float change in angle, in radians
 */
float calcDeltaTheta(std::shared_ptr<TrackingWheel>& tracker1, std::shared_ptr<TrackingWheel>& tracker2) {
    const float numerator = tracker1->getDistanceDelta(false) - tracker2->getDistanceDelta(false);
    const float denominator = tracker1->getOffset() - tracker2->getOffset();
    return numerator / denominator;
}

/**
 * @brief Calculate the change in heading given a vector of imus
 *
 * @note positive change in counterclockwise
 *
 * @param gyros vector of Gyro shared pointers
 * @return float the average change in heading
 */
float calcDeltaTheta(std::vector<std::shared_ptr<Gyro>>& gyros) {
    float deltaTheta = 0;
    for (const auto& gyro : gyros) deltaTheta += gyro->getRotationDelta() / gyros.size();
    return deltaTheta;
}

float calcDeltaTheta(std::vector<std::shared_ptr<pros::GPS>>& gpsArg) {
    float deltaTheta = 0;
    for (const auto& gps : gpsArg) deltaTheta += gps->get_heading() / gpsArg.size();
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
    // 4. GPS Sensor
    // 5. Drivetrain motor encoders
    float theta = this->pose.theta;
    if (this->gyros.size() > 0) { // calculate heading with imus if we have enough
        theta += calcDeltaTheta(this->gyros);
    } else if (horizontals.size() > 1) { // calculate heading with horizontal tracking wheels if we have enough
        theta += calcDeltaTheta(this->horizontals.at(0), this->horizontals.at(1));
    } else if (verticals.size() > 1) { // calculate heading with vertical tracking wheels if we have enough
        theta += calcDeltaTheta(this->verticals.at(0), this->verticals.at(1));
    } else if (drivetrain.size() > 1) { // calculate heading with drivetrain motor encoders
        theta += calcDeltaTheta(this->drivetrain.at(0), this->drivetrain.at(1));
    } else if (gps.size() > 0) { // calculate heading with GPS if we have enough
        theta += calcDeltaTheta(this->gps);
    } else {
        infoSink()->error("Odom calculation failure! Not enough sensors to calculate heading");
        return;
    }
    const float deltaTheta = theta - this->pose.theta;
    const float avgTheta = this->pose.theta + deltaTheta / 2;

    // calculate local change in position
    Pose local(0, 0, deltaTheta);
    // set sinDTheta2 to 1 if deltaTheta is 0. Simplifies local position calculations.
    const float sinDTheta2 = (deltaTheta == 0) ? 1 : 2 * std::sin(deltaTheta / 2);

    // calculate local y position
    if (horizontals.size() > 0) {
        for (auto& tracker : this->horizontals) {
            // prevent divide by 0
            const float radius = (deltaTheta == 0) ? tracker->getDistanceDelta()
                                                   : tracker->getDistanceDelta() / deltaTheta + tracker->getOffset();
            local.y += sinDTheta2 * radius / this->horizontals.size();
        }
    } else if (gps.size() > 0) {
        float totalypos = 0;
        for (auto& gps : this->gps) { totalypos += gps->get_position().y; }
        float averageYPos = totalypos / this->gps.size();
        local.y = averageYPos;
    } else { // output a warning if there are no available sensors to calculate local y
        infoSink()->warn("No horizontal tracking wheels or GPS! Assuming y movement is 0");
    }

    // calculate local x position
    if (this->verticals.size() > 0) { // use dedicated tracking wheels if we have any
        for (auto& tracker : this->verticals) {
            const float radius = (deltaTheta == 0) ? tracker->getDistanceDelta()
                                                   : tracker->getDistanceDelta() / deltaTheta + tracker->getOffset();
            local.x += sinDTheta2 * radius / this->verticals.size();
        }
    } else if (this->drivetrain.size() > 0) { // use motor encoders if we have no dedicated tracking wheels
        for (auto& motor : this->drivetrain) {
            const float radius = (deltaTheta == 0) ? motor->getDistanceDelta()
                                                   : motor->getDistanceDelta() / deltaTheta + motor->getOffset();
            local.x += sinDTheta2 * radius / this->drivetrain.size();
        }
    } else if (this->gps.size() > 0) {
        float totalxpos = 0;
        for (auto& gps : this->gps) { totalxpos += gps->get_position().x; }
        float averageXPos = totalxpos / this->gps.size();
        local.x = averageXPos;
    } else { // output a warning if there are no available sensors to calculate local x
        infoSink()->warn("No vertical tracking wheels or GPS! Assuming y movement is 0");
    }

    // calculate global position
    this->pose += local.rotate(avgTheta);
}
}; // namespace lemlib