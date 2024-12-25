#include "lemlib/tracking/TrackingWheelOdom.hpp"
#include "LemLog/logger/logger.hpp"

static logger::Helper helper("lemlib/odom/tracking_wheel_odom");

namespace lemlib {
TrackingWheel::TrackingWheel(Encoder* encoder, Length diameter, Length offset)
    : encoder(encoder),
      diameter(diameter),
      offset(offset) {}

Length TrackingWheel::getDistanceTraveled() {
    // get encoder angle
    const Angle angle = encoder->getAngle();
    // error checking
    if (angle == from_stDeg(INFINITY)) return from_m(INFINITY);
    if (encoder->setAngle(0_stDeg) != 0) return from_m(INFINITY);
    // return the length traveled using the arc-length formula
    return M_PI * diameter * to_stRad(angle);
}

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

// The implementation below is mostly based off of
// the document written by 5225A (Pilons)
// Here is a link to the original document
// http://thepilons.ca/wp-content/uploads/2018/10/Tracking.pdf
void TrackingWheelOdometry::update(Time period) {
    Time prevTime = from_msec(pros::millis());
    // run until the task has been notified, which will probably never happen
    while (pros::Task::notify_take(true, 0) == 0) {
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
    helper.log(logger::Level::INFO, "tracking task stopped");
}

TrackingWheelOdometry::~TrackingWheelOdometry() { m_task->notify(); }
}; // namespace lemlib