#pragma once

#include "Angle.hpp"
#include "units/Vector2D.hpp"

namespace units {
/**
 * @class Pose
 *
 * @brief A class that represents a position and orientation in 2D space
 *
 * This class inherits from V2Position, which is identical to Vector2D<Length>. The only difference here
 * is that Pose has an additional member variable, orientation.
 */
class Pose : public V2Position {
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
         * This constructor initializes orientation to 0
         *
         * @param v position
         */
        Pose(V2Position v);

        /**
         * @brief Construct a new Pose object
         *
         * @param v position
         * @param orientation orientation
         */
        Pose(V2Position v, Angle orientation);

        /**
         * @brief Construct a new Pose object
         *
         * This constructor initializes orientation to 0
         *
         * @param x x position
         * @param y y position
         */
        Pose(Length x, Length y);

        /**
         * @brief Construct a new Pose object
         *
         * @param x x position
         * @param y y position
         * @param orientation orientation
         */
        Pose(Length x, Length y, Angle orientation);

        /**
         * @brief Get the orientation
         *
         * @return Angle orientation
         */
        Angle getOrientation();

        /**
         * @brief Set the orientation
         *
         * @param orientation orientation
         */
        void setOrientation(Angle orientation);
    protected:
        Angle orientation; /** Orientation */
};

/**
 * @class VelocityPose
 *
 * @brief A class that represents a linear velocity and angular velocity in 2D space
 *
 * This class inherits from V2Velocity, which is identical to Vector2D<LinearVelocity>. The only difference here
 * is that VelocityPose has an additional member variable, angularVelocity.
 */
class VelocityPose : public V2Velocity {
    public:
        /**
         * @brief Construct a new VelocityPose object
         *
         * This constructor initializes x velocity, y velocity, and angular velocity to 0
         */
        VelocityPose();

        /**
         * @brief Construct a new VelocityPose object
         *
         * This constructor initializes angular velocity to 0
         *
         * @param v linear velocity vector
         */
        VelocityPose(V2Velocity v);

        /**
         * @brief Construct a new VelocityPose object
         *
         * @param v linear velocity vector
         * @param angularVelocity angular velocity
         */
        VelocityPose(V2Velocity v, AngularVelocity angularVelocity);

        /**
         * @brief Construct a new VelocityPose object
         *
         * This constructor initializes angular velocity to 0
         *
         * @param x x linear velocity
         * @param y y linear velocity
         */
        VelocityPose(LinearVelocity x, LinearVelocity y);

        /**
         * @brief Construct a new VelocityPose object
         *
         * @param x x linear velocity
         * @param y y linear velocity
         * @param angularVelocity  angular velocity
         */
        VelocityPose(LinearVelocity x, LinearVelocity y, AngularVelocity angularVelocity);

        /**
         * @brief Get the angular velocity
         *
         * @return AngularVelocity
         */
        AngularVelocity getAngularVelocity();

        /**
         * @brief Set the angular velocity
         *
         * @param angularVelocity angular velocity
         */
        void setAngularVelocity(AngularVelocity angularVelocity);
    protected:
        AngularVelocity angularVelocity; /** Angular velocity */
};

/**
 * @class AccelerationPose
 *
 * @brief A class that represents a linear acceleration and angular acceleration in 2D space
 *
 * This class inherits from V2Acceleration, which is identical to Vector2D<LinearAcceleration>. The only difference here
 * is that AccelerationPose has an additional member variable, angularAcceleration.
 */
class AccelerationPose : public V2Acceleration {
    public:
        /**
         * @brief Construct a new AccelerationPose object
         *
         * This constructor initializes x acceleration, y acceleration, and angular acceleration to 0
         */
        AccelerationPose();

        /**
         * @brief Construct a new AccelerationPose object
         *
         * This constructor initializes angular acceleration to 0
         *
         * @param v linear acceleration vector
         */
        AccelerationPose(V2Acceleration v);

        /**
         * @brief Construct a new AccelerationPose object
         *
         * @param v linear acceleration vector
         * @param angularAcceleration angular acceleration
         */
        AccelerationPose(V2Acceleration v, AngularAcceleration angularAcceleration);

        /**
         * @brief Construct a new AccelerationPose object
         *
         * This constructor initializes angular acceleration to 0
         *
         * @param x x linear acceleration
         * @param y y linear acceleration
         */
        AccelerationPose(LinearAcceleration x, LinearAcceleration y);

        /**
         * @brief Construct a new AccelerationPose object
         *
         * @param x x linear acceleration
         * @param y y linear acceleration
         * @param angularAcceleration  angular acceleration
         */
        AccelerationPose(LinearAcceleration x, LinearAcceleration y, AngularAcceleration angularAcceleration);

        /**
         * @brief Get the angular acceleration
         *
         * @return AngularAcceleration
         */
        AngularAcceleration getAngularAcceleration();

        /**
         * @brief Set the angular acceleration
         *
         * @param angularAcceleration angular acceleration
         */
        void setAngularAcceleration(AngularAcceleration angularAcceleration);
    protected:
        AngularAcceleration angularAcceleration; /** Angular acceleration */
};
} // namespace units