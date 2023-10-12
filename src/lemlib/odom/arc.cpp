#include <errno.h>
#include "lemlib/util.hpp"
#include "lemlib/odom/arc.hpp"

/**
 * Arc odom constructor
 *
 * Vectors of tracking wheels and imus are used, so any number of sensors
 * could be used
 */
lemlib::Arc::Arc(std::vector<TrackingWheel>& verticals, std::vector<TrackingWheel>& horizontals,
                 std::vector<pros::Imu>& imus)
    : verticals(verticals),
      horizontals(horizontals),
      imus(imus) {}

/**
 * Calibrate sensors used for odometry
 *
 * Calibrating tracking wheels are easy. It happens instantly.
 * For imu's though, its harder. They need 3 seconds to calibrate. In order
 * to save time, we should calibrate all available imus at the same time. But we
 * still need to remove them if they fail after 5 attempts
 */
void lemlib::Arc::calibrate(int timeout) {
    // calibrate tracking wheels
    for (TrackingWheel& tracker : verticals) tracker.reset();
    for (TrackingWheel& tracker : horizontals) tracker.reset();
    for (pros::Imu& imu : imus) imu.reset();
    std::vector<pros::Imu> newImus;
    while (true) {
        for (auto it = imus.begin(); it < imus.end(); it++) {
            if (!it->is_calibrating() && checkErrno()) {
                newImus.push_back(*it);
                it = imus.erase(it);
            }
        }
        pros::delay(10);
    }
}
