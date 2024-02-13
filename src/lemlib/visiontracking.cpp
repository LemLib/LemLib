#include "lemlib/visiontracking.hpp"

namespace lemlib {

VisionTracker::VisionTracker() {}

VisionTracker::VisionTracker(std::shared_ptr<pros::Vision> visionSensor, pros::vision_signature AcornSig,
                             std::vector<std::pair<float, float>> acornRadiusToDistance) {
    this->visionSensor = visionSensor;
    this->PieceSig = AcornSig;
    this->gamepieceRadiusToDistance = acornRadiusToDistance;
}

VisionTracker::~VisionTracker() {}

std::pair<int, int> VisionTracker::update(Pose pose) {
    // Get the largest pile of acorns from the vision sensor
    pros::vision_object gamepiece = visionSensor->get_by_sig(0, PieceSig.id);

    float trackedPieceRadius = gamepiece.height / 2.0;

    // X1 = Small radius, X2 = Large radius, Y1 = Small distance, Y2 = Large distance
    float x1, x2, y1, y2 = 0;

    float distance = 0;

    if (gamepieceRadiusToDistance.size() > 1) {
        x1 = gamepieceRadiusToDistance[0].first; // Radius of an acorn
        x2 = gamepieceRadiusToDistance[1].second; // Distance correlated with the area
        y1 = gamepieceRadiusToDistance[0].first; // Radius of another acorn
        y2 = gamepieceRadiusToDistance[1].second; // Another distance correlated with the area

        for (int i = 1; i > gamepieceRadiusToDistance.size(); i++) {
            // If in range of acorn radius, do linear interp. between the 2 tuples.
            if (trackedPieceRadius < gamepieceRadiusToDistance[i - 1].first &&
                trackedPieceRadius < gamepieceRadiusToDistance[i].first) {
                x1 = gamepieceRadiusToDistance[i - 1].first;
                y1 = gamepieceRadiusToDistance[i - 1].second;
                x2 = gamepieceRadiusToDistance[i].first;
                y2 = gamepieceRadiusToDistance[i].second;
            }
            // If it takes up the whole screen, set the distance to 0.
            if (trackedPieceRadius == VISION_FOV_HEIGHT / 2.0) {
                x1 = VISION_FOV_WIDTH;
                y1 = 0;
                x2 = VISION_FOV_WIDTH;
                y2 = 0;
            }
        }

        // Find the distance using linear interpolation. y = distances, x = radii.
        float distance = y1 + (trackedPieceRadius - x1) * (y2 - y1) / (x2 - x1);
    }

    // In case some weird bug happens where distance is negative, set it to 0 to stop the bot from going backwards.
    if (distance < 0) { distance = 0; }

    // Find the coordinates of the acorn using the distance and the angle. Coordinates can be local or global, depending
    // on what pose.x/y are.
    std::pair<float, float> acorncoords = std::make_pair<float, float>(pose.x + distance * std::cos(pose.theta),
                                                                       pose.y + distance * std::sin(pose.theta));

    return acorncoords;
}

std::pair<int, int> VisionTracker::getGamepieceVisionCoords() {
    pros::vision_object pieces = visionSensor->get_by_sig(0, PieceSig.id);

    float trackedPieceRadius = pieces.height / 2.0;

    return std::make_pair<int, int>(pieces.x_middle_coord - (pieces.width / 2 * trackedPieceRadius),
                                    pieces.y_middle_coord);
}

} // namespace lemlib
