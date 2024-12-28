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
 * @brief Sanitize data from sensors used for odometry
 *
 * This function checks if the data given equals INFINITY, and if it does, the data and the sensor
 * which reported the data is removed from their respective vectors.
 *
 * @tparam T the type of the data, assumed to be a unit (e.g Length, Angle)
 * @tparam U the type of the sensors
 *
 * @param data the data to sanitize
 * @param sensors the sensors the data was gotten from
 */
template <typename T, typename U> static T sanitizeData(const std::vector<T>& data, std::vector<U>& sensors) {
    // go through all the data
    for (int i = 0; i < data.size(); ++i) {
        const T element = data.at(i);
        // check if the data is good
        if (element.internal() == INFINITY) {
            // output different warning messages based on the sensor type
            if constexpr (std::is_same_v<U, TrackingWheel>) {
                helper.log(logger::Level::WARN, "Failed to get data from tracking wheel, removing tracking wheel!");
            } else if constexpr (std::is_same_v<U, Imu*>) {
                helper.log(logger::Level::WARN, "Failed to get data from IMU, removing IMU!");
            }
            // remove the sensor
            sensors.erase(sensors.begin() + i);
            --i;
        } else return element;
    }
    // if no sensor data is available, return 0
    return T(0.0);
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
static Angle calculateHeading(std::vector<TrackingWheel>& trackingWheels) {
    // check that there are enough tracking wheels
    if (trackingWheels.size() < 2) { return from_stDeg(INFINITY); }
    // get data
    const Length distance1 = trackingWheels.at(0).getDistanceTraveled();
    const Length distance2 = trackingWheels.at(1).getDistanceTraveled();
    const Length offset1 = trackingWheels.at(0).getOffset();
    const Length offset2 = trackingWheels.at(1).getOffset();
    // check that the offsets aren't the same
    if (offset1 == offset2) {
        helper.log(logger::Level::WARN, "Tracking wheel offsets are equal, removing one tracking wheel!");
        trackingWheels.erase(trackingWheels.begin() + 1);
        return calculateHeading(trackingWheels);
    }
    // check for errors
    if (distance1.internal() == INFINITY) {
        helper.log(logger::Level::WARN, "Failed to get data from tracking wheel, removing tracking wheel!");
        trackingWheels.erase(trackingWheels.begin());
        return calculateHeading(trackingWheels);
    }
    if (distance2.internal() == INFINITY) {
        helper.log(logger::Level::WARN, "Failed to get data from tracking wheel, removing tracking wheel!");
        trackingWheels.erase(trackingWheels.begin() + 1);
        return calculateHeading(trackingWheels);
    }
    // return the calculated heading
    return from_stRad((distance1 - distance2) / (offset1 - offset2));
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

        // step 1: get sensor data
        // TODO: find some standard library function to make this cleaner
        const std::vector<Length> deltaXs = [&] {
            std::vector<Length> result;
            std::transform(m_horizontalWheels.begin(), m_horizontalWheels.end(), result.begin(),
                           [](TrackingWheel& wheel) { return wheel.getDistanceDelta(); });
            return result;
        }();
        const std::vector<Length> deltaYs = [&] {
            std::vector<Length> result;
            std::transform(m_verticalWheels.begin(), m_verticalWheels.end(), result.begin(),
                           [](TrackingWheel& wheel) { return wheel.getDistanceDelta(); });
            return result;
        }();
        const std::vector<Angle> thetas = [&] {
            std::vector<Angle> result;
            std::transform(m_Imus.begin(), m_Imus.end(), result.begin(), [](Imu* imu) { return imu->getRotation(); });
            return result;
        }();

        // step 2: sanitize data
        sanitizeData(thetas, m_Imus);
        // if there are no measurements available, then assume delta is 0
        const Length deltaX = sanitizeData(deltaXs, m_horizontalWheels);
        const Length deltaY = sanitizeData(deltaYs, m_verticalWheels);

        // step 3: calculate change in heading
        const Angle theta = [&] {
            while (true) {
                if (!thetas.empty()) { // prefer to use IMU
                    return thetas.at(0);
                } else if (m_horizontalWheels.size() >= 2) { // use horizontal encoders
                    const Angle result = calculateHeading(m_horizontalWheels);
                    if (result.internal() == INFINITY) continue;
                    else return result;
                } else if (m_verticalWheels.size() >= 2) { // use vertical encoders
                    const Angle result = calculateHeading(m_verticalWheels);
                    if (result.internal() == INFINITY) continue;
                    else return result;
                } else { // we don't have enough data
                    helper.log(logger::Level::ERROR, "Can't calculate heading, not enough data!");
                    break;
                }
            }
            return from_stDeg(INFINITY);
        }();
        const Angle deltaTheta = theta - m_pose.theta();
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