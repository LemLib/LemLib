#pragma once

#include <utility>
#include <vector>
#include <optional>
#include "lemlib/pose.hpp"
#include "lemlib/pid.hpp"
#include "lemlib/asset.hpp"
#include "lemlib/chassis/structs.hpp"

namespace lemlib {
/**
 * @brief Waypoint class. Derived from Pose. Has speed field
 */
class Waypoint : public Pose {
    public:
        /**
         * @brief Construct a new Waypoint
         *
         * @param x
         * @param y
         * @param theta
         * @param speed
         */
        Waypoint(float x, float y, float theta, float speed)
            : Pose(x, y, theta),
              speed(speed) {}

        float speed;
};

/**
 * @brief Abstract class for a movement
 */
class Movement {
    public:
        /**
         * @brief Construct a new Movement object
         */
        Movement();

        /**
         * @brief Update the movement
         *
         * @note This function should be called in a loop
         * @note Units are in inches and radians. Angle 0 is right, and increases counterclockwise
         *
         * @param pose the position of the chassis
         *
         * @return std::pair<int, int> left and right motor power respectively. 128 means ignore
         */
        virtual std::pair<int, int> update(Pose pose) = 0;

        /**
         * @brief Get the distance travelled during the movement
         *
         * @return float
         */
        float getDist() { return dist; }
    protected:
        int compState;
        int state = 0; // 0 = in progress, 1 = settling, 2 = done
        float dist = 0;
};

/**
 * @brief Turn class. Derived from Movement
 */
class Turn : protected Movement {
    public:
        /**
         * @brief Construct a new Turn movement
         *
         * @param angularSettings the angular PID gains
         * @param target the target heading. Radians, 0 is right, increases counterclockwise
         * @param timeout the maximum time the robot can spend moving
         * @param maxSpeed the maximum speed the robot can turn at. 127 by default
         */
        Turn(ChassisController_t angularSettings, float target, int timeout, int maxSpeed = 127);

        /**
         * @brief Construct a new Turn movement
         *
         * @param angularSettings the angular PID gains
         * @param target the target position
         * @param timeout the maximum time the robot can spend moving
         * @param reversed whether the robot should face the point with its back or front. False by default
         * @param maxSpeed the maximum speed the robot can turn at. 127 by default
         */
        Turn(ChassisController_t angularSettings, Pose target, int timeout, bool reversed = false, int maxSpeed = 127);

        /**
         * @brief Update the movement
         *
         * @note This function should be called in a loop
         * @note Units are in inches and radians. Angle 0 is right, and increases counterclockwise
         *
         * @param pose the position of the chassis
         *
         * @return std::pair<int, int> left and right motor power respectively. 128 means movement is done
         */
        std::pair<int, int> update(Pose pose) override;
    private:
        ChassisController_t angularSettings;
        std::optional<float> targetHeading = std::nullopt;
        std::optional<Pose> targetPose = std::nullopt;
        int timeout;
        bool reversed;
        int maxSpeed;

        FAPID angularPID;
};

/**
 * @brief Boomerang movement class. Derived from Movement
 */
class Boomerang : protected Movement {
    public:
        /**
         * @brief Construct a new Boomerang movement
         *
         * @param drive drivetrain settings
         * @param angularSettings the angular PID gains for the boomerang controller
         * @param linearSettings the linear PID gains for the boomerang controller
         * @param target the target pose
         * @param timeout longest time the robot can spend moving
         * @param forwards whether the robot should move forwards or backwards. true for forwards (default), false for
         * backwards
         * @param lead the lead parameter. Determines how curved the robot will move. 0.6 by default (0 < lead < 1)
         * @param chasePower higher values make the robot move faster but causes more overshoot on turns. 0 makes it
         * default to global value
         * @param maxSpeed the maximum speed the robot can move at. 127 by default
         */
        Boomerang(Drivetrain_t drive, ChassisController_t angularSettings, ChassisController_t linearSettings,
                  Pose target, int timeout, bool forwards = true, float chasePower = 0, float lead = 0.6,
                  int maxSpeed = 127);

        /**
         * @brief Update the movement
         *
         * @note This function should be called in a loop
         * @note Units are in inches and radians. Angle 0 is right, and increases counterclockwise
         *
         * @param pose the position of the chassis
         *
         * @return std::pair<int, int> left and right motor power respectively. 128 means movement is done
         */
        std::pair<int, int> update(Pose pose) override;
    private:
        Drivetrain_t drive;
        ChassisController_t angularSettings;
        ChassisController_t linearSettings;
        Pose target;
        int timeout;
        bool forwards;
        float chasePower;
        float lead;
        int maxSpeed;

        FAPID angularPID;
        FAPID linearPID;
};

/**
 * @brief Pure Pursuit class. Derived from Movement
 */
class PurePursuit : protected Movement {
    public:
        /**
         * @brief Construct a new Pure Pursuit movement
         *
         * @param drive drivetrain settings
         * @param path const reference to the asset containing the path
         * @param lookahead the lookahead distance. Units in inches. Recommended value is 15, but can be changed if
         * needed
         * @param timeout the maximum time the robot can spend moving
         * @param forwards whether the chassis should move forwards or backwards. True by default
         * @param maxSpeed the maximum speed the robot can move at. 127 by default
         */
        PurePursuit(Drivetrain_t drive, const asset& path, float lookahead, int timeout, bool forwards = true,
                    int maxSpeed = 127);

        /**
         * @brief Update the movement
         *
         * @note This function should be called in a loop
         * @note Units are in inches and radians. Angle 0 is right, and increases counterclockwise
         *
         * @param pose the position of the chassis
         *
         * @return std::pair<int, int> left and right motor power respectively. 128 means movement is done
         */
        std::pair<int, int> update(Pose pose) override;
    private:
        Drivetrain_t drive;
        std::vector<Waypoint> path;
        int startTime;
        float lookahead;
        int timeout;
        bool forwards;
        int maxSpeed;
};
} // namespace lemlib