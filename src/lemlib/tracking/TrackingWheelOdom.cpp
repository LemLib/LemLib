#include "lemlib/tracking/TrackingWheelOdom.hpp"
#include "lemlog/logger/logger.hpp"

static logger::Helper helper("lemlib/odom/tracking_wheel_odom");

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
    if (m_task == std::nullopt) { // start the task
        m_task = pros::Task([this, period] { this->update(period); });
        helper.log(logger::Level::INFO, "tracking task started!");
    } else {
        helper.log(logger::Level::WARN, "tried to start tracking task, but it has already been started!");
    }
}

void TrackingWheelOdometry::update(Time period) {
    Time prevTime = from_msec(pros::millis());
    while (true) {
        const Time now = from_msec(pros::millis());
        const Time deltaTime = now - prevTime;
        // if current time - previous time > timeout
        // then set previous time to current time
        // this is to prevent the tracking task updating multiple times
        // with no delay in between
        if (deltaTime > period) prevTime = now;
        uint32_t dummyPrevTime = to_msec(prevTime);
        pros::Task::delay_until(&dummyPrevTime, to_msec(period));
        prevTime = from_msec(dummyPrevTime);
    }
}
}; // namespace lemlib