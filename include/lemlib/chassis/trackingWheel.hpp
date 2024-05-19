#pragma once

#include "pros/motors.hpp"
#include "pros/motor_group.hpp"
#include "pros/adi.hpp"
#include "pros/rotation.hpp"

namespace lemlib {

/**
 * @brief A namespace representing the size of omniwheels.
 */
namespace Omniwheel {
constexpr float NEW_2 = 2.125;
constexpr float NEW_275 = 2.75;
constexpr float OLD_275 = 2.75;
constexpr float NEW_275_HALF = 2.744;
constexpr float OLD_275_HALF = 2.74;
constexpr float NEW_325 = 3.25;
constexpr float OLD_325 = 3.25;
constexpr float NEW_325_HALF = 3.246;
constexpr float OLD_325_HALF = 3.246;
constexpr float NEW_4 = 4;
constexpr float OLD_4 = 4.18;
constexpr float NEW_4_HALF = 3.995;
constexpr float OLD_4_HALF = 4.175;
} // namespace Omniwheel

class TrackingWheel {
    public:
        /**
         * @brief Create a new tracking wheel
         *
         * @param encoder the optical shaft encoder to use
         * @param wheelDiameter the diameter of the wheel
         * @param distance distance between the tracking wheel and the center of rotation in inches
         * @param gearRatio gear ratio of the tracking wheel, defaults to 1
         *
         * @b Example
         * @code {.cpp}
         * // Create a new optical shaft encoder on ports 'A' and 'B'
         * // this sensor is also reversed
         * pros::adi::Encoder verticalEncoder('A', 'B', true);
         * // create a new vertical tracking wheel
         * // it's using a new 2.75 inch wheel
         * // it's also 5 inches away from the tracking center. This tracking wheel is to the left
         * // of the tracking center, so we use a negative distance. If it was to the right of the
         * // tracking center, we would use a positive distance
         * lemlib::TrackingWheel verticalTrackingWheel(&verticalEncoder, lemlib::Omniwheel::NEW_275, -5);
         * // create a new optical shaft encoder on ports `C` and `D`
         * // this sensor is not reversed
         * pros::adi::Encoder horizontalEncoder('C', 'D', false);
         * // create a new horizontal tracking wheel
         * // it's using an old 3.25 inch wheel
         * // it's also 2 inches away from the tracking center. This tracking wheel is to the back
         * // of the tracking center, so we use a negative distance. If it was to the front of the
         * // tracking center, we would use a positive distance
         * // this wheel also has a 5:3 gear ratio
         * lemlib::TrackingWheel horizontalTrackingWheel(&horizontalEncoder, lemlib::Omniwheel::OLD_325, -2, 5.0/3.0);
         * @endcode
         */
        TrackingWheel(pros::adi::Encoder* encoder, float wheelDiameter, float distance, float gearRatio = 1);
        /**
         * @brief Create a new tracking wheel
         *
         * @param encoder the v5 rotation sensor to use
         * @param wheelDiameter the diameter of the wheel
         * @param distance distance between the tracking wheel and the center of rotation in inches
         * @param gearRatio gear ratio of the tracking wheel, defaults to 1
         *
         * @b Example
         * @code {.cpp}
         * // Create a new rotation sensor on port 1
         * // this sensor is not reversed
         * pros::Rotation verticalEncoder(1, false);
         * // create a new vertical tracking wheel
         * // it's using a new 2.75 inch wheel
         * // it's also 5 inches away from the tracking center. This tracking wheel is to the left
         * // of the tracking center, so we use a negative distance. If it was to the right of the
         * // tracking center, we would use a positive distance
         * lemlib::TrackingWheel verticalTrackingWheel(&verticalEncoder, lemlib::Omniwheel::NEW_275, -5);
         * // create a new rotation sensor on port 2
         * // this sensor is reversed
         * pros::Rotation horizontalEncoder(2, true);
         * // create a new horizontal tracking wheel
         * // it's using an old 3.25 inch wheel
         * // it's also 2 inches away from the tracking center. This tracking wheel is to the back
         * // of the tracking center, so we use a negative distance. If it was to the front of the
         * // tracking center, we would use a positive distance
         * // this wheel also has a 5:3 gear ratio
         * lemlib::TrackingWheel horizontalTrackingWheel(&horizontalEncoder, lemlib::Omniwheel::OLD_325, -2, 5.0/3.0);
         * @endcode
         */
        TrackingWheel(pros::Rotation* encoder, float wheelDiameter, float distance, float gearRatio = 1);
        /**
         * @brief Create a new tracking wheel
         *
         * @param motors the motor group to use
         * @param wheelDiameter the diameter of the wheel
         * @param distance half the track width of the drivetrain in inches
         * @param rpm theoretical maximum rpm of the drivetrain wheels
         *
         * @b Example
         * @code {.cpp}
         * // Create motor objects
         * // note its important to include the motor gearset otherwise the gear ratio will be incorrect
         * pros::Motor lF(-5, pros::E_MOTOR_GEARSET_06); // left front motor. port 5, reversed
         * pros::Motor lM(4, pros::E_MOTOR_GEARSET_06); // left middle motor. port 4
         * pros::Motor lB(-3, pros::E_MOTOR_GEARSET_06); // left back motor. port 3, reversed
         * // create a motor group for the left side of the drivetrain
         * pros::MotorGroup leftMotors({lF, lM, lB});
         * // Create a new tracking wheel using the left motor group
         * // it's using an old 4 inch wheel
         * // and its distance is half the track width of the drivetrain
         * // distance is also negative because the left drive side is to the left of the tracking center
         * // if it was to the right of the tracking center, we would use a positive distance
         * // the rpm is 360
         * lemlib::TrackingWheel leftTrackingWheel(&leftMotors, lemlib::Omniwheel::OLD_4, -5, 360);
         * @endcode
         */
        TrackingWheel(pros::MotorGroup* motors, float wheelDiameter, float distance, float rpm);
        /**
         * @brief Reset the tracking wheel position to 0
         *
         * If you are using odometry provided by LemLib, this will automatically be called when
         * the chassis is calibrated
         *
         * @b Example
         * @code {.cpp}
         * void initialize() {
         *     exampleTrackingWheel.reset();
         *     // this will now return 0 inches traveled
         *     exampleTrackingWheel.getDistanceTraveled();
         * }
         * @endcode
         */
        void reset();
        /**
         * @brief Get the distance traveled by the tracking wheel
         *
         * @return float distance traveled in inches
         *
         * @b Example
         * @code {.cpp}
         * void initialize() {
         *     while (true) {
         *         // print the distance traveled by the tracking wheel to the terminal
         *         std::cout << "distance: " << exampleTrackingWheel.getDistanceTraveled() << std::endl;
         *         pros::delay(10);
         *     }
         * }
         */
        float getDistanceTraveled();
        /**
         * @brief Get the offset of the tracking wheel from the center of rotation
         *
         * @return float offset in inches
         *
         * @b Example
         * @code {.cpp}
         * void initialize() {
         *     // create a tracking wheel with an offset of 0.5 inches
         *     lemlib::TrackingWheel exampleTrackingWheel(&exampleEncoder, lemlib::Omniwheel::NEW_275, 0.5);
         *     // this prints 0.5 to the terminal, the offset of the tracking wheel
         *     std::cout << "offset: " << exampleTrackingWheel.getOffset() << std::endl;
         * }
         * @endcode
         */
        float getOffset();
        /**
         * @brief Get the type of tracking wheel
         *
         * @return int - 1 if motor group, 0 otherwise
         *
         * @b Example
         * @code {.cpp}
         * void initialize() {
         *     // create a tracking wheel that uses a rotation sensor
         *     lemlib::TrackingWheel exampleTrackingWheel(&exampleRotationSensor, lemlib::Omniwheel::NEW_275, 0.5);
         *     // this prints 0 to the terminal, because it uses a rotation sensor. If it used a motor group, it would
         *     // have returned 1
         *     std::cout << "type: " << exampleTrackingWheel.getType() << std::endl;
         * }
         * @endcode
         */
        int getType();
    private:
        float diameter;
        float distance;
        float rpm;
        pros::adi::Encoder* encoder = nullptr;
        pros::Rotation* rotation = nullptr;
        pros::MotorGroup* motors = nullptr;
        float gearRatio = 1;
};
} // namespace lemlib