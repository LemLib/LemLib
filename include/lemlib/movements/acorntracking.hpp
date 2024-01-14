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

        uint32_t AcornSig;

        AcornTracker();

    public:
        AcornTracker(pros::Vision* visionSensor, const uint32_t sig_id);

        ~AcornTracker();

        bool setAcornDistanceToHeight(std::vector<std::pair<float, float>>& acornDistanceToHeight);

        std::pair<int, int> update(Pose pose) override;

        float getDist() override;

};

} // namespace lemlib