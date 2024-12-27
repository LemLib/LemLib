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
template <typename T, typename U> static void sanitizeData(std::vector<T>& data, std::vector<U>& sensors) {
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
            // remove the data and the sensor
            data.erase(data.begin() + i);
            sensors.erase(sensors.begin() + i);
            --i;
        }
    }
}

/**
 * @brief calculate the heading given at least 2 tracking wheels
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
    return from_stRad(((distance1 - distance2) / (offset1 - offset2)).internal());
}

/*
 * The implementation below is based off of
 * the document written by 5225A (Pilons)
 * http://thepilons.ca/wp-content/uploads/2018/10/Tracking.pdf
 */
void TrackingWheelOdometry::update(Time period) {
    // reset the tracking wheels
    for (TrackingWheel& trackingWheel : m_horizontalWheels) { trackingWheel.reset(); }
    for (TrackingWheel& trackingWheel : m_verticalWheels) { trackingWheel.reset(); }

    // record the previous time, used for consistent loop timings
    Time prevTime = from_msec(pros::millis());
    // run until the task has been notified, which will probably never happen
    while (pros::Task::notify_take(true, 0) == 0) {
        const Time now = from_msec(pros::millis());
        const Time deltaTime = now - prevTime;

        // step 1: get sensor data
        std::vector<Length> deltaXs;
        std::vector<Length> deltaYs;
        std::vector<Angle> thetas;
        for (TrackingWheel& wheel : m_horizontalWheels) { deltaXs.push_back(wheel.getDistanceDelta()); }
        for (TrackingWheel& wheel : m_verticalWheels) { deltaYs.push_back(wheel.getDistanceDelta()); }
        for (Imu* imu : m_Imus) { thetas.push_back(imu->getRotation()); }

        // step 2: sanitize data
        sanitizeData(deltaXs, m_horizontalWheels);
        sanitizeData(deltaYs, m_verticalWheels);
        sanitizeData(thetas, m_Imus);
        // if there are no measurements available, then assume delta is 0
        const Length deltaX = deltaXs.empty() ? 0_m : deltaXs.at(0);
        const Length deltaY = deltaYs.empty() ? 0_m : deltaYs.at(0);

        // step 3: calculate change in heading
        Angle theta = 0_stDeg;
    calculateDeltaTheta: // forgive me using GOTO, but it's just for error handling
        if (!thetas.empty()) { // prefer to use IMU to find the change in heading
            theta = thetas.at(0);
        } else if (m_horizontalWheels.size() >= 2) { // use horizontal encoders to calculate the change in heading
            const Angle result = calculateHeading(m_horizontalWheels);
            if (result.internal() == INFINITY) goto calculateDeltaTheta;
            else theta = result;
        } else if (m_verticalWheels.size() >= 2) { // use vertical encoders to calculate the change in heading
            const Angle result = calculateHeading(m_verticalWheels);
            if (result.internal() == INFINITY) goto calculateDeltaTheta;
            else theta = result;
        } else { // we don't have enough data for odom
            helper.log(logger::Level::ERROR, "Can't calculate heading, not enough data!");
            break;
        }
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