#include "lemlib/api.hpp"
#include "lemlib/pose.hpp"
#include "pros/vision.hpp"

/*
 *      Acorn Tracking, written by Bradley Fernandez (Uvuv)
 *
 *
 */

namespace lemlib {

class AcornTracker { // : public Pose {
    private:
    protected:
        std::vector<std::pair<float, float>> acornRadiusToDistance;

        std::shared_ptr<pros::Vision> visionSensor = nullptr;

        pros::vision_signature AcornSig;

        AcornTracker();

        ~AcornTracker();
    public:
        AcornTracker(std::shared_ptr<pros::Vision> visionSensor, pros::vision_signature AcornSig,
                     std::vector<std::pair<float, float>> acornRadiusToDistance);

        bool setAcornRadiusToDistance(std::vector<std::pair<float, float>> acornRadiusToDistance);

        std::pair<int, int> update(Pose pose); // override
};

} // namespace lemlib