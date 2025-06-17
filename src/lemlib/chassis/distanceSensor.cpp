#include "lemlib/chassis/distanceSensor.hpp"

namespace lemlib {

DistanceSensor::DistanceSensor(pros::v5::Distance* sensor, float offset)
    : sensor(sensor), offset(offset) {}

float DistanceSensor::getOffset() {
    return offset;
}

float DistanceSensor::getDistance() {
    return 0.03937008*(sensor->get());  // or sensor->get_distance() depending on your API
}

}
