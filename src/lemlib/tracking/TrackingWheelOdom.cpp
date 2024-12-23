#include "lemlib/tracking/TrackingWheelOdom.hpp"

namespace lemlib {
TrackingWheel::TrackingWheel(Encoder* encoder, Length diameter, Length offset)
    : encoder(encoder),
      diameter(diameter),
      offset(offset) {}

TrackingWheelOdometry::TrackingWheelOdometry(std::vector<Imu*> imus, std::vector<TrackingWheel> verticalWheels,
                                             std::vector<TrackingWheel> horizontalWheels)
    : m_Imus(imus),
      m_verticalWheels(verticalWheels),
      m_horizontalWheels(horizontalWheels) {}

void TrackingWheelOdometry::startTask(Time period) {
    // check if the task has been started yet
    if (m_task == std::nullopt) {
        // start the task
        m_task = pros::Task([this, period] {
            // call the update member function periodically
            uint32_t prev = pros::millis();
            while (true) {
                const uint32_t now = pros::millis();
                const int64_t delta = now - prev;
                // if current time - previous time > timeout
                // then set previous time to current time
                // this is to prevent the tracking task updating multiple times
                // with no delay in between
                if (delta > to_msec(period)) prev = now;
                this->update();
                pros::Task::delay_until(&prev, to_msec(period));
            }
        });
    }
}
}; // namespace lemlib