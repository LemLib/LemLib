#include "lemlib/tracking/TrackingWheelOdom.hpp"
#include "LemLog/logger/logger.hpp"

static logger::Helper helper("lemlib/odom/tracking_wheel_odom");

namespace lemlib {
TrackingWheel::TrackingWheel(Encoder* encoder, Length diameter, Length offset)
    : m_encoder(encoder),
      m_diameter(diameter),
      m_offset(offset),
      m_lastTotal(to_stRad(encoder->getAngle()) * M_PI * diameter) {}

Length TrackingWheel::getDistanceTraveled() { return to_stRad(m_encoder->getAngle()) * M_PI * m_diameter; }

Length TrackingWheel::getDistanceDelta() {
    // calculate delta
    const Length total = this->getDistanceTraveled();
    const Length delta = total - m_lastTotal;
    m_lastTotal = total;
    // return the delta
    return delta;
}

Length TrackingWheel::getOffset() { return m_offset; }

int TrackingWheel::reset() { return m_encoder->setAngle(0_stDeg); }

TrackingWheelOdometry::TrackingWheelOdometry(std::vector<Imu*> imus, std::vector<TrackingWheel> verticalWheels,
                                             std::vector<TrackingWheel> horizontalWheels)
    : m_Imus(imus),
      m_verticalWheels(verticalWheels),
      m_horizontalWheels(horizontalWheels) {}

void TrackingWheelOdometry::startTask(Time period) {
    // check if the task has been started yet
    if (m_task == std::nullopt) { // start the task
        m_task = pros::Task([this, period] { this->update(period); });
        helper.log(logger::Level::INFO, "Tracking task started!");
    } else {
        helper.log(logger::Level::WARN, "Tried to start tracking task, but it has already been started!");
    }
}

/**
 * @brief Find position delta given tracking wheels
 *
 * This function checks if the data given equals INFINITY, and if it does, the tracking wheel
 * which reported the data is removed its vectors.
 *
 * @param sensors the sensors to get data from
 *
 * @return Length the position delta
 */
static Length findLateralDelta(std::vector<TrackingWheel>& sensors) {
    for (int i = 0; i < sensors.size(); ++i) {
        const Length data = sensors.at(i).getDistanceDelta();
        if (data.internal() == INFINITY) { // error checking
            sensors.erase(sensors.begin() + i);
            --i;
            helper.log(logger::Level::WARN, "Failed to get data from tracking wheel, removing tracking wheel!");
        } else return data;
    }
    // return 0 if no data was found
    return 0_m;
}

/**
 * @brief calculate the heading given at least 2 tracking wheels
 *
 * TODO: use std::expected
 *
 * @param trackingWheels vector of tracking wheels which will be used to calculate the heading
 *
 * @return INFINITY there's not enough tracking wheels to calculate the heading
 * @return Angle the heading
 */
static std::optional<Angle> calculateHeading(std::vector<TrackingWheel>& trackingWheels, Angle offset) {
    // check that there are enough tracking wheels
    if (trackingWheels.size() < 2) return std::nullopt;
    // get data
    const Length distance1 = trackingWheels.at(0).getDistanceTraveled();
    const Length distance2 = trackingWheels.at(1).getDistanceTraveled();
    const Length offset1 = trackingWheels.at(0).getOffset();
    const Length offset2 = trackingWheels.at(1).getOffset();
    // check that the offsets aren't the same
    if (offset1 == offset2) {
        helper.log(logger::Level::WARN, "Tracking wheel offsets are equal, removing one tracking wheel!");
        trackingWheels.erase(trackingWheels.begin() + 1);
        return calculateHeading(trackingWheels, offset);
    }
    // check for errors
    if (distance1.internal() == INFINITY) {
        helper.log(logger::Level::WARN, "Failed to get data from tracking wheel, removing tracking wheel!");
        trackingWheels.erase(trackingWheels.begin());
        return calculateHeading(trackingWheels, offset);
    }
    if (distance2.internal() == INFINITY) {
        helper.log(logger::Level::WARN, "Failed to get data from tracking wheel, removing tracking wheel!");
        trackingWheels.erase(trackingWheels.begin() + 1);
        return calculateHeading(trackingWheels, offset);
    }
    // return the calculated heading
    return offset + from_stRad((distance1 - distance2) / (offset1 - offset2));
}

/*
 * The implementation below is based off of
 * the document written by 5225A (Pilons)
 * http://thepilons.ca/wp-content/uploads/2018/10/Tracking.pdf
 */
void TrackingWheelOdometry::update(Time period) {
    // reset the tracking wheels
    for (TrackingWheel& trackingWheel : m_horizontalWheels) trackingWheel.reset();
    for (TrackingWheel& trackingWheel : m_verticalWheels) trackingWheel.reset();
    // set the IMU so it's ready the same value odom is set to
    for (Imu* imu : m_Imus) imu->setRotation(m_pose.orientation);

    // record the previous time, used for consistent loop timings
    Time prevTime = from_msec(pros::millis());
    // run until the task has been notified, which will probably never happen
    while (pros::Task::notify_take(true, 0) == 0) {
        const Time now = from_msec(pros::millis());
        const Time deltaTime = now - prevTime;

        // step 1: get tracking wheel deltas
        const Length deltaX = findLateralDelta(m_horizontalWheels);
        const Length deltaY = findLateralDelta(m_verticalWheels);

        // step 2: calculate heading
        const std::optional<Angle> theta = [&]() -> std::optional<Angle> {
            // get data from IMUs
            for (int i = 0; i < m_Imus.size(); ++i) {
                const Angle data = m_Imus.at(i)->getRotation();
                if (data.internal() == INFINITY) { // error checking
                    m_Imus.erase(m_Imus.begin() + i);
                    --i;
                    helper.log(logger::Level::WARN, "Failed to get data from IMU, removing IMU!");
                } else return m_offset + data;
            }
            // if we don't have IMU data, try using horizontal tracking wheels
            const std::optional<Angle> result = calculateHeading(m_horizontalWheels, m_offset);
            if (result != std::nullopt) return result;
            // try using vertical tracking wheels if there aren't any horizontal wheels available
            return calculateHeading(m_verticalWheels, m_offset);
        }();
        if (theta == std::nullopt) { // error checking
            helper.log(logger::Level::ERROR, "Not enough sensors available!");
            break;
        }

        const Angle deltaTheta = *theta - m_pose.theta();
        const Angle averageTheta = m_pose.theta() + deltaTheta / 2;

        // if current time - previous time > timeout
        // then set previous time to current time
        // this is to prevent the tracking task updating multiple times
        // with no delay in between
        if (deltaTime > period) prevTime = now;
        uint32_t dummyPrevTime = to_msec(prevTime);
        pros::Task::delay_until(&dummyPrevTime, to_msec(period));
        prevTime = from_msec(dummyPrevTime);
    }

    helper.log(logger::Level::INFO, "Tracking task stopped!");
}

TrackingWheelOdometry::~TrackingWheelOdometry() { m_task->notify(); }
}; // namespace lemlib