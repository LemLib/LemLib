#pragma once

#include "Angle.hpp"
#include "units/Vector2D.hpp"
#include "units/units.hpp"

namespace units {
/**
 * @class Pose
 *
 * @brief A class that represents a position and orientation in 2D space
 *
 * This class inherits from Vector2D<Length / derivatives>, and has an additional Orientation component of type <Angle /
 * derivatives>, where derivatives is a power of time.
 */
template <typename derivatives> class AbstractPose
    : public Vector2D<Divided<Length, Exponentiated<Time, derivatives>>> {
        using Len = Divided<Length, Exponentiated<Time, derivatives>>;
        using Vector = Vector2D<Len>;
    public:
        /**
         * @brief Construct a new Pose object
         *
         * This constructor initializes x, y, and orientation to 0
         */
        AbstractPose() : Vector(), orientation(0.0) {}

        /**
         * @brief Construct a new Pose object
         *
         * This constructor initializes orientation to 0
         *
         * @param v position
         */
        AbstractPose(Vector v) : Vector(v), orientation(0.0) {}

        /**
         * @brief Construct a new Pose object
         *
         * @param v position
         * @param orientation orientation
         */
        AbstractPose(Vector v, Divided<Angle, Exponentiated<Time, derivatives>> orientation)
            : Vector(v), orientation(orientation) {}

        /**
         * @brief Construct a new Pose object
         *
         * This constructor initializes orientation to 0
         *
         * @param x x position
         * @param y y position
         */
        AbstractPose(Len x, Len y) : Vector(x, y), orientation(0.0) {}

        /**
         * @brief Construct a new Pose object
         *
         * @param x x position
         * @param y y position
         * @param orientation orientation
         */
        AbstractPose(Len x, Len y, Divided<Angle, Exponentiated<Time, derivatives>> orientation)
            : Vector(x, y), orientation(orientation) {}

        /**
         * @brief Get the orientation
         *
         * @return Angle orientation
         */
        Divided<Angle, Exponentiated<Time, derivatives>> getOrientation() { return orientation; }

        /**
         * @brief Set the orientation
         *
         * @param orientation orientation
         */
        void setOrientation(Divided<Angle, Exponentiated<Time, derivatives>> orientation) {
            this->orientation = orientation;
        }
    protected:
        Divided<Angle, Exponentiated<Time, derivatives>> orientation; /** Orientation */
};

// Position Pose (Length, Angle)
using Pose = AbstractPose<std::ratio<0>>;
// Velocity Pose (Length / Time, Angle / Time)
using VelocityPose = AbstractPose<std::ratio<1>>;
// AccelerationPose (Length / Time^2, Angle / Time^2)
using AccelerationPose = AbstractPose<std::ratio<2>>;

} // namespace units