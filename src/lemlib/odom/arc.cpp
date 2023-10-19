#include "lemlib/util.hpp"
#include "lemlib/timer.hpp"
#include "lemlib/logger/logger.hpp"
#include "lemlib/odom/arc.hpp"

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
lemlib::ArcOdom::ArcOdom(std::vector<TrackingWheel>& verticals, std::vector<TrackingWheel>& horizontals,
                         std::vector<std::shared_ptr<Gyro>>& gyros)
    : verticals(verticals),
      horizontals(horizontals),
      gyros(gyros) {}

/**
 * Calibrate the sensors
 *
 * We have to calibrate tracking wheels and imus. We calibrate them all and remove any that fail
 * calibration. The encoders will output errors if they fail to calibrate.
 */
void lemlib::ArcOdom::calibrate() {
    // calibrate vertical tracking wheels
    for (auto it = verticals.begin(); it != verticals.end(); it++) {
        if (it->reset()) {
            infoSink()->warn("Vertical tracker at offset {} failed calibration!", it->getOffset());
            verticals.erase(it);
        }
    }

    // calibrate horizontal tracking wheels
    for (auto it = horizontals.begin(); it != horizontals.end(); it++) {
        if (it->reset()) {
            infoSink()->warn("Horizontal tracker at offset {} failed calibration!", it->getOffset());
            horizontals.erase(it);
        }
    }

    // calibrate gyros
    for (auto it = gyros.begin(); it != gyros.end(); it++) (**it).calibrate();
    Timer timer(3000); // try calibrating gyros for 3000 ms
    while (!timer.isDone()) {
        for (auto it = gyros.begin(); it != gyros.end(); it++) { // continuously calibrate in case of failure
            if (!(**it).isCalibrating() && !(**it).isCalibrated()) (**it).calibrate();
        }
        pros::delay(10);
    }

    // if a gyro failed to calibrate, output an error and erase the gyro
    for (auto it = gyros.begin(); it != gyros.end(); it++) {
        if (!(**it).isCalibrated()) {
            infoSink()->warn("IMU on port {} failed to calibrate! Removing", (**it).getPort());
            gyros.erase(it);
        }
    }
}

/**
 * Update Arc Odom
 *
 * Tracking through arcs works through estimating the robot's change in position between
 * angles as an arc, rather than a straight line, improving accuracy.
 *
 * This alg can either use tracking wheels to calculate angle, or preferably an imu.
 * Theoretically you can get better performance with tracking wheels, but this is very
 * difficult to achieve.
 *
 * 5225A has published a fantastic paper on this odom algorithm:
 * http://thepilons.ca/wp-content/uploads/2018/10/Tracking.pdf
 */
void lemlib::ArcOdom::update() {
    // calculate change in heading
    float heading = pose.theta;
    if (gyros.size() > 0) { // calculate heading with imus if we have enough
        std::vector<float> angles;
        for (auto it = gyros.begin(); it != gyros.end(); it++) angles += it->getAngleDelta();
        heading += avg(angles);
    } else if (verticals.size() > 1) { // calculate heading with vertical tracking wheels if we have enough
        heading += (verticals.at(0).getDistanceDelta() - verticals.at(1).getDistanceDelta()) /
                   (verticals.at(0).getOffset() - verticals.at(1).getOffset());
    } else if (horizontals.size() > 1) { // calculate heading with horizontal tracking wheels if we have enough
        heading += (horizontals.at(0).getDistanceDelta() - horizontals.at(1).getDistanceDelta()) /
                   (horizontals.at(0).getOffset() - horizontals.at(1).getOffset());
    }

    // calculate the heading of the robot
    // Priority:
    // 1. IMU
    // 2. Horizontal tracking wheels
    // 3. Vertical tracking wheels
    float heading = pose.theta;
    // calculate heading with inertial sensor if it exists
    if (sensors.imu != nullptr) heading += deltaImu;
    // else, use horizontal tracking wheels if they both exist
    else if (sensors.horizontal1 != nullptr && sensors.horizontal2 != nullptr)
        heading += (deltaHorizontal1 - deltaHorizontal2) /
                   (sensors.horizontal1->getOffset() - sensors.horizontal2->getOffset());
    else
        heading +=
            (deltaVertical1 - deltaVertical2) / (sensors.vertical1->getOffset() - sensors.vertical2->getOffset());
    float deltaHeading = heading - pose.theta;
    float avgHeading = pose.theta + deltaHeading / 2;

    // choose tracking wheels to use
    TrackingWheel* verticalWheel = sensors.vertical1;
    TrackingWheel* horizontalWheel = nullptr;
    // horizontal tracking wheels
    if (sensors.horizontal1 != nullptr) horizontalWheel = sensors.horizontal1;
    if (sensors.horizontal2 != nullptr) horizontalWheel = sensors.horizontal2;
    // get raw values
    float rawVertical = 0;
    float rawHorizontal = 0;
    rawVertical = verticalWheel->getDistance();
    if (horizontalWheel != nullptr) rawHorizontal = horizontalWheel->getDistance();
    // get offsets
    float horizontalOffset = 0;
    float verticalOffset = 0;
    verticalOffset = verticalWheel->getOffset();
    if (horizontalWheel != nullptr) horizontalOffset = horizontalWheel->getOffset();

    // calculate change in x and y
    float deltaX = 0;
    float deltaY = 0;
    deltaY = rawVertical - prevVertical;
    if (horizontalWheel != nullptr) deltaX = rawHorizontal - prevHorizontal;
    prevVertical = rawVertical;
    prevHorizontal = rawHorizontal;

    // calculate local x and y
    float localX = 0;
    float localY = 0;
    if (deltaHeading == 0) { // prevent divide by 0
        localX = deltaX;
        localY = deltaY;
    } else {
        localX = 2 * sin(deltaHeading / 2) * (deltaX / deltaHeading + horizontalOffset);
        localY = 2 * sin(deltaHeading / 2) * (deltaY / deltaHeading + verticalOffset);
    }

    // calculate global x and y
    pose.x += localY * sin(avgHeading);
    pose.y += localY * cos(avgHeading);
    pose.x += localX * -cos(avgHeading);
    pose.y += localX * sin(avgHeading);
    pose.theta = heading;
}
