#include "lemlib/api.hpp"
#include "lemlib/pose.hpp"
#include "pros/vision.hpp"

/*
 *      Vision Tracking, written by Bradley Fernandez (Uvuv)
 *
 *
 */

namespace lemlib {

class VisionTracker { // : public Pose {
    private:
    protected:
        std::vector<std::pair<float, float>> gamepieceRadiusToDistance;

        std::shared_ptr<pros::Vision> visionSensor = nullptr;

        pros::vision_signature PieceSig;

        VisionTracker();

        ~VisionTracker();
    public:
        VisionTracker(std::shared_ptr<pros::Vision> visionSensor, pros::vision_signature AcornSig,
                      std::vector<std::pair<float, float>> acornRadiusToDistance);

        bool setGamepieceRadiusToDistance(std::vector<std::pair<float, float>> acornRadiusToDistance);

        std::pair<int, int> update(Pose pose); // override

        /**
         * @brief Get the coordinates of the target acorn on the vision sensor's output.
         *
         * @return std::pair<int, int>
         */
        std::pair<int, int> getGamepieceVisionCoords();
};

} // namespace lemlib