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
        if (it->reset()) verticals.erase(it); // remove the tracking wheel if calibration failed
    }
    // loop through horizontal tracking wheels
    for (auto it = horizontals.begin(); it != horizontals.end(); it++) {
        if (it->reset()) horizontals.erase(it); // remove the tracking wheel if calibration failed
    }

    // calibrate gyros
    for (auto it = gyros.begin(); it != gyros.end(); it++) (**it).calibrate();
    Timer timer(3000); // try calibrating gyros for 3000 ms
    while (!timer.isDone()) {
        for (auto it = gyros.begin(); it != gyros.end(); it++) { // loop through all IMUs
            if (!(**it).isCalibrating() && !(**it).isCalibrated()) (**it).calibrate();
        }
        pros::delay(10);
    }
    // if a gyro failed to calibrate, output an error
    for (auto it = gyros.begin(); it != gyros.end(); it++) {
        if (!(**it).isCalibrated()) infoSink()->error("Error: IMU failed to calibrate");
    }
}

void lemlib::ArcOdom::update() {}
