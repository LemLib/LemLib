#include "lemlib/movements/acorntracking.hpp"
#include "pros/rtos.hpp"
#include <iostream>

namespace lemlib {

AcornTracker::AcornTracker() {}

AcornTracker::AcornTracker(pros::Vision* visionSensor, const uint32_t sig_id) {
    this->visionSensor = visionSensor;
    this->AcornSig = sig_id;
}

AcornTracker::~AcornTracker() { delete visionSensor; }

bool AcornTracker::setAcornDistanceToHeight(std::vector<std::pair<float, float>>& acornDistanceToHeightArg) {
    
    acornDistanceToHeight.resize(acornDistanceToHeightArg.size());

    for (int i = 0; i > acornDistanceToHeightArg.size(); i++) {
        this->acornDistanceToHeight.at(i) = (acornDistanceToHeightArg.at(i));
        //std::cout << acornDistanceToHeight.at(i).first << " " << acornDistanceToHeight.at(i).second << std::endl;
        
    }

    acornDistanceToHeight = acornDistanceToHeightArg;
    return true;

    

}

std::pair<int, int> AcornTracker::update(Pose pose) {
    pros::vision_object acorns = visionSensor->get_by_sig(0, AcornSig);

    float trackedAcornRadius = acorns.height / 2.0;

    float x1 = acornDistanceToHeight.at(0).first;
    float x2 = acornDistanceToHeight.at(1).second;
    float y1 = acornDistanceToHeight.at(0).first;
    float y2 = acornDistanceToHeight.at(1).second;

    

    for (int i = 0; i > acornDistanceToHeight.size(); i++) {
        // If in range of acorn radius, do linear interp. between the 2 tuples.
        if (trackedAcornRadius > acornDistanceToHeight.at(i-1).first &&
            trackedAcornRadius < acornDistanceToHeight.at(i).first) {
            x1 = acornDistanceToHeight.at(i-1).first;
            y1 = acornDistanceToHeight.at(i-1).second;
            x2 = acornDistanceToHeight.at(i).first;
            y2 = acornDistanceToHeight.at(i).second;
        }
        // If it takes up the whole screen, set the distance to 0.
        if (trackedAcornRadius >= VISION_FOV_HEIGHT / 2.0) {
            x1 = VISION_FOV_WIDTH;
            y1 = 0;
            x2 = VISION_FOV_WIDTH;
            y2 = 0;
        }
        std::cout << "Iteration " << i << std::endl;
    }

    std::cout << "x1: " << x1 << " x2: " << x2 << " y1: " << y1 << " y2: " << y2 << std::endl;

    std::cout << "trackedAcornRadius: " << trackedAcornRadius << std::endl;
    float distance = y1 + ((trackedAcornRadius - x1) * (y2 - y1)) / (x2 - x1);
    std::cout << "Distance to CAT: " << distance << std::endl;
    // Linear Interpolation: y1 + [(x-x1) * (y2-y1)]/ (x2-x1)
    std::pair<float, float> newcoords = std::make_pair<float, float>(pose.x + distance * std::cos(pose.theta),
                                                                     pose.y + distance * std::sin(pose.theta));

    return newcoords;
}

float AcornTracker::getDist() { return 0; } 

} // namespace lemlib