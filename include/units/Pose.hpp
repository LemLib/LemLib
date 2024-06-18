#include "units/Vector2D.hpp"

namespace units {
/**
 * @class Pose
 *
 * @brief A class that represents a position and orientation in 2D space
 *
 * This class inherits from V2Position, which is identical to Vector2D<Length>. The only difference here
 * is that Pose has an additional member variable, theta, which represents orientation.
 */
class Pose : public V2Position {
    protected:
        Angle theta; /** Orientation */
    public:
        /**
         * @brief Construct a new Pose object
         *
         * This constructor initializes x, y, and orientation to 0
         */
        Pose();

        /**
         * @brief Construct a new Pose object
         *
         * This constructor initializes x, y to the given values and orientation to 0
         *
         * @param v position
         */
        Pose(V2Position v);

        /**
         * @brief Construct a new Pose object
         *
         * @param v position
         * @param h orientation
         */
        Pose(V2Position v, Angle h);

        /**
         * @brief Construct a new Pose object
         *
         * @param nx x position
         * @param ny y position
         */
        Pose(Length nx, Length ny);

        /**
         * @brief Construct a new Pose object
         *
         * @param nx x position
         * @param ny y position
         * @param nh orientation
         */
        Pose(Length nx, Length ny, Angle nh);

        /**
         * @brief Get the orientation
         *
         * @return Angle orientation
         */
        Angle getTheta();

        /**
         * @brief Set the orientation
         *
         * @param h orientation
         */
        void setTheta(Angle h);
};

} // namespace units