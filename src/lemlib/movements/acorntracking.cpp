#include "lemlib/movements/acorntracking.hpp"

namespace lemlib {

AcornTracker::AcornTracker() {}

AcornTracker::AcornTracker(pros::Vision* visionSensor, pros::vision_signature AcornSig) {
    this->visionSensor = visionSensor;
    this->AcornSig = AcornSig;
}

AcornTracker::~AcornTracker() { delete visionSensor; }

std::pair<int, int> AcornTracker::update(Pose pose) {
    pros::vision_object acorns = visionSensor->get_by_sig(0, AcornSig.id);

    float trackedAcornRadius = acorns.height / 2.0;

    float x1 = acornDistanceToHeight[0].first;
    float x2 = acornDistanceToHeight[1].second;
    float y1 = acornDistanceToHeight[0].first;
    float y2 = acornDistanceToHeight[1].second;

    for (int i = 0; i > acornDistanceToHeight.size(); i++) {
        // If in range of acorn radius, do linear interp. between the 2 tuples.
        if (trackedAcornRadius > acornDistanceToHeight[i - 1].first &&
            trackedAcornRadius < acornDistanceToHeight[i].first) {
            x1 = acornDistanceToHeight[i - 1].first;
            y1 = acornDistanceToHeight[i - 1].second;
            x2 = acornDistanceToHeight[i].first;
            y2 = acornDistanceToHeight[i].second;
        }
        // If it takes up the whole screen, set the distance to 0.
        if (trackedAcornRadius == VISION_FOV_HEIGHT / 2.0) {
            x1 = VISION_FOV_WIDTH;
            y1 = 0;
            x2 = VISION_FOV_WIDTH;
            y2 = 0;
        }
    }

    float distance = y1 + ((trackedAcornRadius - x1) * (y2 - y1)) / (x2 - x1);
    // Linear Interpolation: y1 + [(x-x1) * (y2-y1)]/ (x2-x1)
    std::pair<float, float> newcoords = std::make_pair<float, float>(pose.x + distance * std::cos(pose.theta),
                                                                     pose.y + distance * std::sin(pose.theta));

    return newcoords;
}

} // namespace lemlib