#pragma once

#include "hardware/Encoder/Encoder.hpp"
#include "hardware/Port.hpp"
#include "pros/rotation.hpp"

namespace lemlib {
/**
 * @brief Encoder implementation for the V5 Rotation sensor
 *
 */
class V5RotationSensor : public Encoder {
    public:
        /**
         * @brief Construct a new V5 Rotation Sensor
         *
         * @param port the signed port of the rotation sensor. Positive if the sensor is reversed, Negative otherwise
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     // rotation sensor on port 1, which is reversed
         *     lemlib::V5RotationSensor encoder(-1);
         * }
         * @endcode
         */
        V5RotationSensor(ReversibleSmartPort port);
        /**
         * @brief V5RotationSensor copy constructor
         *
         * Because pros::Mutex does not have a copy constructor, an explicit
         * copy constructor for the V5RotationSensor is necessary
         *
         * @param other the V5RotationSensor to copy
         */
        V5RotationSensor(const V5RotationSensor& other);
        /**
         * @brief Create a new V5 Rotation Sensor
         *
         * @param encoder the pros::Rotation object to use
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::V5RotationSensor encoder = lemlib::V5RotationSensor::from_pros_rot(pros::Rotation(1));
         * }
         * @endcode
         */
        static V5RotationSensor from_pros_rot(pros::Rotation encoder);
        /**
         * @brief whether the V5 Rotation Sensor is connected
         *
         * @return 0 if its not connected
         * @return 1 if it is connected
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::V5RotationSensor encoder = pros::Rotation(1);
         *     const int result = encoder.isConnected();
         *     if (result == 1) {
         *         std::cout << "Encoder is connected!" << std::endl;
         *     } else if (result == 0) {
         *         std::cout << "Encoder is not connected!" << std::endl;
         *     } else {
         *         std::cout << "Error checking if encoder is connected!" << std::endl;
         *     }
         * }
         * @endcode
         */
        int32_t isConnected() const override;
        /**
         * @brief Get the relative angle measured by the V5 Rotation Sensor
         *
         * The relative angle measured by the V5 Rotation Sensor is the angle of the encoder relative to the last time
         * the V5 Rotation Sensor was reset. As such, it is unbounded.
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENXIO: the port is not within the range of valid ports (1-21)
         * ENODEV: the port cannot be configured as an V5 Rotation sensor
         *
         * @return Angle the relative angle measured by the encoder
         * @return INFINITY if there is an error, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::V5RotationSensor encoder = pros::Rotation(1);
         *     const Angle angle = encoder.getAngle();
         *     if (angle == INFINITY) {
         *         std::cout << "Error getting relative angle!" << std::endl;
         *     } else {
         *         std::cout << "Relative angle: " << angle.convert(deg) << std::endl;
         *     }
         * }
         * @endcode
         */
        Angle getAngle() const override;
        /**
         * @brief Set the relative angle of the V5 Rotation Sensor
         *
         * This function sets the relative angle of the V5 Rotation Sensor. The relative angle is the number of
         * rotations the V5 Rotation Sensor has measured since the last reset. This function is non-blocking.
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENXIO: the port is not within the range of valid ports (1-21)
         * ENODEV: the port cannot be configured as an V5 Rotation sensor
         *
         * @param angle the relative angle to set the measured angle to
         * @return 0 on success
         * @return INT_MAX on failure, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::V5RotationSensor encoder = pros::Rotation(1);
         *     if (encoder.setAngle(0_stDeg) == 0) {
         *         std::cout << "Relative angle set!" << std::endl;
         *         std::cout < "Relative angle: " << encoder.getAngle().convert(deg) << std::endl; // outputs 0
         *     } else {
         *         std::cout << "Error setting relative angle!" << std::endl;
         *     }
         * }
         * @endcode
         */
        int32_t setAngle(Angle angle) override;
        /**
         * @brief returns whether the V5 Rotation Sensor is reversed or not
         *
         * @return 0 if it is not reversed
         * @return 1 if it is reversed
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::V5RotationSensor encoder = pros::Rotation(1);
         *     const int result = encoder.isReversed();
         *     if (result == 0) {
         *         std::cout << "Rotation sensor is not reversed" << std::endl;
         *     } else {
         *         std::cout << "Rotation sensor is reversed" << std::endl;
         *     }
         * }
         * @endcode
         */
        int32_t isReversed() const;
        /**
         * @brief Set whether the V5 Rotation Sensor is reversed or not
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENXIO: the port is not within the range of valid ports (1-21)
         * ENODEV: the port cannot be configured as an V5 Rotation sensor
         *
         * @return 0 on success
         * @return INT_MAX on failure, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::V5RotationSensor encoder = pros::Rotation(1);
         *     // reverse the encoder
         *     encoder.setReversed(true);
         * }
         * @endcode
         */
        int32_t setReversed(bool reversed);
    private:
        mutable pros::Mutex m_mutex;
        Angle m_offset = 0_stRot;
        bool m_reversed;
        int m_port;
};
} // namespace lemlib