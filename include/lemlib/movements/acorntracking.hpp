#include "lemlib/api.hpp"
#include "lemlib/pose.hpp"
#include "pros/vision.hpp"

/*
 *      Acorn Tracking, written by Bradley Fernandez (Uvuv)
 *
 *      Derived from Pose class.
 *
 */

namespace lemlib {

    class AcornTracker : public Movement {
    private:

    protected:

        std::vector<std::pair<float, float>> acornDistanceToHeight;

        pros::Vision* visionSensor = nullptr;

        pros::vision_signature AcornSig;

        AcornTracker();

        ~AcornTracker();

    public:

        AcornTracker(pros::Vision* visionSensor, pros::vision_signature AcornSig);

        bool setAcornDistanceToHeight(std::vector<std::pair<float, float>> acornDistanceToHeight);

        std::pair<int, int> update(Pose pose) override;

    };

}