#include "lemlib/tracking/TrackingWheelOdom.hpp"
#include "LemLog/logger/Helper.hpp"
#include "hardware/Encoder/ADIEncoder.hpp"
#include "hardware/Encoder/V5RotationSensor.hpp"
#include "units/Vector2D.hpp"

static logger::Helper helper("lemlib/odom/tracking_wheel_odom");

namespace lemlib {
TrackingWheel::TrackingWheel(Encoder* encoder,
                             Length diameter,
                             Length offset,
                             Number ratio)
    : m_encoder(encoder),
      m_diameter(diameter),
      m_offset(offset),
      m_ratio(ratio),
      m_lastTotal(to_stRot(encoder->getAngle()) * M_PI * diameter * m_ratio) {}

TrackingWheel::TrackingWheel(ReversibleSmartPort port,
                             Length diameter,
                             Length offset,
                             Number ratio)
    : m_encoder(new V5RotationSensor(port)),
      m_diameter(diameter),
      m_offset(offset),
      m_ratio(ratio),
      m_lastTotal(to_stRot(m_encoder->getAngle()) * M_PI * diameter * m_ratio),
      m_deallocate(true) {}

TrackingWheel::TrackingWheel(ADIPort topPort,
                             ADIPort bottomPort,
                             bool reversed,
                             Length diameter,
                             Length offset,
                             Number ratio)
    : m_encoder(new ADIEncoder(topPort, bottomPort, reversed)),
      m_diameter(diameter),
      m_offset(offset),
      m_ratio(ratio),
      m_lastTotal(to_stRot(m_encoder->getAngle()) * M_PI * diameter * m_ratio),
      m_deallocate(true) {}

TrackingWheel::TrackingWheel(SmartPort expanderPort,
                             ADIPort topPort,
                             ADIPort bottomPort,
                             bool reversed,
                             Length diameter,
                             Length offset,
                             Number ratio)
    : m_encoder(new ADIEncoder(expanderPort, topPort, bottomPort, reversed)),
      m_diameter(diameter),
      m_offset(offset),
      m_ratio(ratio),
      m_lastTotal(to_stRot(m_encoder->getAngle()) * M_PI * diameter * m_ratio),
      m_deallocate(true) {}

Length TrackingWheel::getDistanceTraveled() {
    return to_stRot(m_encoder->getAngle()) * M_PI * m_diameter;
}

Length TrackingWheel::getDistanceDelta() {
    // calculate delta
    const Length total = this->getDistanceTraveled();
    const Length delta = total - m_lastTotal;
    m_lastTotal = total;
    // return the delta
    return delta;
}

Length TrackingWheel::getOffset() {
    return m_offset;
}

int TrackingWheel::reset() {
    return m_encoder->setAngle(0_stDeg);
}

TrackingWheel::~TrackingWheel() {
    if (m_deallocate) delete m_encoder;
}

TrackingWheelOdometry::TrackingWheelOdometry(
  std::vector<IMU*> imus,
  std::vector<TrackingWheel*> verticalWheels,
  std::vector<TrackingWheel*> horizontalWheels)
    : m_Imus(imus),
      m_verticalWheels(verticalWheels),
      m_horizontalWheels(horizontalWheels) {}

units::Pose TrackingWheelOdometry::getPose() {
    return m_pose;
}

void TrackingWheelOdometry::setPose(units::Pose pose) {
    m_offset += pose.orientation - m_pose.orientation;
    m_pose = pose;
}

void TrackingWheelOdometry::startTask(Time period) {
    // check if the task has been started yet
    if (m_task == std::nullopt) { // start the task
        m_task = pros::Task([this, period] {
            this->update(period);
        });
        helper.log(logger::Level::INFO, "Tracking task started!");
    } else {
        helper.log(
          logger::Level::WARN,
          "Tried to start tracking task, but it has already been started!");
    }
}

/**
 * @brief struct representing data from a tracking wheel
 *
 */
struct TrackingWheelData {
    Length distance; /** the distance delta reported by the tracking wheel */
    Length offset; /** the offset of the tracking wheel used to measure the
                      distance */
};

/**
 * @brief Find position delta given tracking wheels
 *
 * This function checks if the data given equals INFINITY, and if it does, the
 * tracking wheel which reported the data is removed its vectors.
 *
 * @param sensors the sensors to get data from
 *
 * @return LateralDelta the position delta
 */
static TrackingWheelData
findLateralDelta(std::vector<TrackingWheel*>& sensors) {
    for (int i = 0; i < sensors.size(); ++i) {
        TrackingWheel* sensor = sensors.at(i);
        const Length data = sensor->getDistanceDelta();
        if (data.internal() == INFINITY) { // error checking
            sensors.erase(sensors.begin() + i);
            --i;
            helper.log(
              logger::Level::WARN,
              "Failed to get data from tracking wheel, removing " "trackin" "g " "wh" "ee" "l" "!");
        } else {
            return { data, sensor->getOffset() };
        }
    }
    // return 0 if no data was found
    return { 0_m, 0_m };
}

/**
 * @brief calculate the heading given at least 2 tracking wheels
 *
 * @param trackingWheels vector of tracking wheels which will be used to
 * calculate the heading
 *
 * @return std::nullopt there's not enough tracking wheels to calculate the
 * heading
 * @return std::optional<Angle> the heading
 */
static std::optional<Angle>
calculateWheelHeading(std::vector<TrackingWheel*>& trackingWheels) {
    // check that there are enough tracking wheels
    if (trackingWheels.size() < 2) return std::nullopt;
    // get data
    const Length distance1 = trackingWheels.at(0)->getDistanceTraveled();
    const Length distance2 = trackingWheels.at(1)->getDistanceTraveled();
    const Length offset1 = trackingWheels.at(0)->getOffset();
    const Length offset2 = trackingWheels.at(1)->getOffset();
    // check that the offsets aren't the same
    if (offset1 == offset2) {
        helper.log(
          logger::Level::WARN,
          "Tracking wheel offsets are equal, removing one tracking wheel!");
        trackingWheels.erase(trackingWheels.begin() + 1);
        return calculateWheelHeading(trackingWheels);
    }
    // check for errors
    if (distance1.internal() == INFINITY) {
        helper.log(
          logger::Level::WARN,
          "Failed to get data from tracking wheel, removing tracking wheel!");
        trackingWheels.erase(trackingWheels.begin());
        return calculateWheelHeading(trackingWheels);
    }
    if (distance2.internal() == INFINITY) {
        helper.log(
          logger::Level::WARN,
          "Failed to get data from tracking wheel, removing tracking wheel!");
        trackingWheels.erase(trackingWheels.begin() + 1);
        return calculateWheelHeading(trackingWheels);
    }
    // return the calculated heading
    return from_stRad((distance1 - distance2) / (offset1 - offset2)) + 90_stDeg;
}

/**
 * @brief calculate the heading given at least 1 IMU
 *
 * @param imus the vector of IMUs to get data from
 *
 * @return std::nullopt there's not IMUs to calculate the heading
 * @return std::optional<Angle> the heading
 */
static std::optional<Angle> calculateIMUHeading(std::vector<IMU*>& imus) {
    for (int i = 0; i < imus.size(); ++i) {
        const Angle data = imus.at(i)->getRotation();
        if (data.internal() == INFINITY) { // error checking
            imus.erase(imus.begin() + i);
            --i;
            helper.log(logger::Level::WARN,
                       "Failed to get data from IMU, removing IMU!");
        } else {
            return data;
        }
    }
    // return nullopt if we couldn't get any data
    return std::nullopt;
}

/*
 * The implementation below is based off of
 * the document written by 5225A (Pilons)
 * http://thepilons.ca/wp-content/uploads/2018/10/Tracking.pdf
 */
void TrackingWheelOdometry::update(Time period) {
    // record the previous time, used for consistent loop timings
    Time prevTime = from_msec(pros::millis());
    // run until the task has been notified, which will probably never happen
    while (pros::Task::notify_take(true, 0) == 0) {
        const Time now = from_msec(pros::millis());
        const Time deltaTime = now - prevTime;

        // step 1: get tracking wheel deltas
        const TrackingWheelData horizontalData =
          findLateralDelta(m_horizontalWheels);
        const TrackingWheelData verticalData =
          findLateralDelta(m_verticalWheels);

        // step 2: calculate heading
        const std::optional<Angle> thetaOpt =
          calculateIMUHeading(m_Imus)
            .or_else(std::bind(&calculateWheelHeading, m_horizontalWheels))
            .or_else(std::bind(&calculateWheelHeading, m_verticalWheels));
        if (thetaOpt == std::nullopt) { // error checking
            helper.log(logger::Level::ERROR, "Not enough sensors available!");
            break;
        }
        const Angle theta = m_offset + *thetaOpt;

        // step 3: calculate change in local coordinates
        const Angle deltaTheta = theta - m_pose.orientation;
        const units::V2Position localPosition = [&] {
            const units::V2Position lateralDeltas = { verticalData.distance,
                                                      horizontalData.distance };
            const units::V2Position lateralOffsets = { verticalData.offset,
                                                       horizontalData.offset };
            if (deltaTheta == 0_stRad) {
                return lateralDeltas; // prevent divide by 0
            } else {
                return 2 * units::sin(deltaTheta / 2) *
                       (lateralDeltas / to_stRad(deltaTheta) + lateralOffsets);
            }
        }();

        // step 4: set global position
        m_pose += localPosition.rotatedBy(m_pose.orientation + deltaTheta / 2);
        m_pose.orientation = theta;

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

TrackingWheelOdometry::~TrackingWheelOdometry() {
    m_task->notify();
}
}; // namespace lemlib
