#include "lemlib/chassis/distanceSensor.hpp"

namespace lemlib {

DistanceSensor::DistanceSensor(pros::v5::Distance* sensor, float offset)
    : sensor(sensor), offset(offset) {}

float DistanceSensor::getOffset() {
    return offset;
}

float DistanceSensor::getDistance() {
    return sensor->get_distance();  // or sensor->get_distance() depending on your API
}

}
