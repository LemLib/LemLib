#pragma once

//#include "pros/motors.hpp"
//#include "pros/motor_group.hpp"
//#include "pros/adi.hpp"
#include "pros/distance.hpp"

namespace lemlib {

class DistanceSensor {
    public:
        
        /**
         * @brief Create a new distance sensor
         *
         * @param sensor the distance sensor port to use
         * @param offset the offset from center line
         *
         * @b Example
         * @code {.cpp}

         * @endcode
         */
        DistanceSensor(pros::Distance* sensor, float offset);
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
        float getDistance();
        /**
         * @brief Get the offset of the tracking wheel from the center of rotation
         *
         * @return float offset in inches
         *
         * @b Example
         * @code {.cpp}
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
    private:
        float distance;
        float offset;
        pros::Distance* sensor;
};
} // namespace lemlib