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
}; // namespace lemlib