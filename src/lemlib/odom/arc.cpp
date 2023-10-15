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
                         std::vector<pros::Imu>& imus)
    : verticals(verticals),
      horizontals(horizontals),
      imus(imus) {}

/**
 * Calibrate the sensors
 *
 * We have to calibrate tracking wheels and imus. We calibrate them all and remove any that fail
 * calibration. The encoders will output errors if they fail to calibrate.
 */
void lemlib::ArcOdom::calibrate() {
    // loop through vertical tracking wheels
    for (auto it = verticals.begin(); it != verticals.end(); it++) {
        if (it->reset()) verticals.erase(it); // remove the tracking wheel if calibration failed
    }
    // loop through horizontal tracking wheels
    for (auto it = horizontals.begin(); it != horizontals.end(); it++) {
        if (it->reset()) horizontals.erase(it); // remove the tracking wheel if calibration failed
    }

    // loop through imus
    for (auto it = imus.begin(); it != imus.end(); it++) it->reset();
    // We will spend a maximum of 3000 seconds calibrating imus before we give up
    // we wil continue to try and calibrate the IMU's until we run out of time
    Timer timer(3000);
    std::vector<pros::Imu> newImus;
    while (!timer.isDone() && imus.size() > 0) { // loop until the timer is done or imus are calibrated
        for (auto it = imus.begin(); it != imus.end(); it++) { // loop through all IMUs
            if (!it->is_calibrating() && !(errno == PROS_ERR || errno == ENODEV || errno == ENXIO)) {
                // erase imu from old vector and add to new one
                imus.erase(it);
                newImus.push_back(*it);
            }
        }
        pros::delay(10);
    }
    // output errors if any imus did not calibrate
    for (auto it = imus.begin(); it != imus.end(); it++) infoSink()->error("Error: IMU failed to calibrate");
    imus = newImus;
}

void lemlib::ArcOdom::update() {}