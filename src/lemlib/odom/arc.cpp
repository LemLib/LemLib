#include "lemlib/odom/arc.hpp"
#include "lemlib/timer.hpp"
#include "lemlib/logger/logger.hpp"

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
    /*
    // get the current sensor values
    float vertical1Raw = 0;
    float vertical2Raw = 0;
    float horizontal1Raw = 0;
    float horizontal2Raw = 0;
    float imuRaw = 0;
    if (sensors.vertical1 != nullptr) vertical1Raw = sensors.vertical1->getDistance();
    if (sensors.vertical2 != nullptr) vertical2Raw = sensors.vertical2->getDistance();
    if (sensors.horizontal1 != nullptr) horizontal1Raw = sensors.horizontal1->getDistance();
    if (sensors.horizontal2 != nullptr) horizontal2Raw = sensors.horizontal2->getDistance();
    if (sensors.imu != nullptr) imuRaw = degToRad(sensors.imu->get_rotation());

    // calculate the change in sensor values
    float deltaVertical1 = vertical1Raw - prevVertical1;
    float deltaVertical2 = vertical2Raw - prevVertical2;
    float deltaHorizontal1 = horizontal1Raw - prevHorizontal1;
    float deltaHorizontal2 = horizontal2Raw - prevHorizontal2;
    float deltaImu = imuRaw - prevImu;

    // update the previous sensor values
    prevVertical1 = vertical1Raw;
    prevVertical2 = vertical2Raw;
    prevHorizontal1 = horizontal1Raw;
    prevHorizontal2 = horizontal2Raw;
    prevImu = imuRaw;

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
    */
}
