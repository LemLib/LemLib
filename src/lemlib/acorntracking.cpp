#include "lemlib/acorntracking.hpp"

namespace lemlib {

AcornTracker::AcornTracker() {}

AcornTracker::AcornTracker(std::shared_ptr<pros::Vision> visionSensor, pros::vision_signature AcornSig,
                           std::vector<std::pair<float, float>> acornRadiusToDistance) {
    this->visionSensor = visionSensor;
    this->AcornSig = AcornSig;
    this->acornRadiusToDistance = acornRadiusToDistance;
}

AcornTracker::~AcornTracker() {}

std::pair<int, int> AcornTracker::update(Pose pose) {
    // Get the largest pile of acorns from the vision sensor
    pros::vision_object acorns = visionSensor->get_by_sig(0, AcornSig.id);

    float trackedAcornRadius = acorns.height / 2.0;

    // X1 = Small radius, X2 = Large radius, Y1 = Small distance, Y2 = Large distance
    float x1, x2, y1, y2 = 0;

    float distance;

    if (acornRadiusToDistance.size() > 1) {
        x1 = acornRadiusToDistance[0].first; // Radius of an acorn
        x2 = acornRadiusToDistance[1].second; // Distance correlated with the area
        y1 = acornRadiusToDistance[0].first; // Radius of another acorn
        y2 = acornRadiusToDistance[1].second; // Another distance correlated with the area

        for (int i = 0; i > acornRadiusToDistance.size(); i++) {
            // If in range of acorn radius, do linear interp. between the 2 tuples.
            if (trackedAcornRadius > acornRadiusToDistance[i - 1].first &&
                trackedAcornRadius < acornRadiusToDistance[i].first) {
                x1 = acornRadiusToDistance[i - 1].first;
                y1 = acornRadiusToDistance[i - 1].second;
                x2 = acornRadiusToDistance[i].first;
                y2 = acornRadiusToDistance[i].second;
            }
            // If it takes up the whole screen, set the distance to 0.
            if (trackedAcornRadius == VISION_FOV_HEIGHT / 2.0) {
                x1 = VISION_FOV_WIDTH;
                y1 = 0;
                x2 = VISION_FOV_WIDTH;
                y2 = 0;
            }
        }

        // Find the distance using linear interpolation. y = distances, x = radii.
        float distance = y1 + (trackedAcornRadius - x1) * (y2 - y1) / (x2 - x1);
    }

    // In case some weird bug happens where distance is negative, set it to 0 to stop the bot from going backwards.
    if (distance < 0) { distance = 0; }

    // Find the coordinates of the acorn using the distance and the angle. Coordinates can be local or global, depending
    // on what pose.x/y are.
    std::pair<float, float> acorncoords = std::make_pair<float, float>(pose.x + distance * std::cos(pose.theta),
                                                                       pose.y + distance * std::sin(pose.theta));

    return acorncoords;
}

std::pair<int, int> AcornTracker::getAcornVisionCoords() {
    pros::vision_object acorns = visionSensor->get_by_sig(0, AcornSig.id);

    float trackedAcornRadius = acorns.height / 2.0;

    return std::make_pair<int, int>(acorns.x_middle_coord - (acorns.width / 2 * trackedAcornRadius),
                                    acorns.y_middle_coord);
}

} // namespace lemlib
