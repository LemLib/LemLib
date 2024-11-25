#pragma once

#include "units/Angle.hpp"

namespace lemlib {
/**
 * @brief abstract class for encoders
 *
 * An encoder is a device that measures the angle of a rotating object. This class is abstract, and must be implemented
 * by a subclass.
 */
class Encoder {
    public:
        /**
         * @brief whether the encoder is connected
         *
         * @return 0 if its not connected
         * @return 1 if it is connected
         * @return INT_MAX if there is an error, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     Encoder* encoder;
         *     const int result = encoder->isConnected();
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
        virtual int isConnected() = 0;
        /**
         * @brief Get the relative angle measured by the encoder
         *
         * The relative angle measured by the encoder is the angle of the encoder relative to the last time the encoder
         * was reset. As such, it is unbounded.
         *
         * @return Angle the relative angle measured by the encoder
         * @return INFINITY if there is an error, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     Encoder* encoder;
         *     const Angle angle = encoder->getAngle();
         *     if (angle == INFINITY) {
         *         std::cout << "Error getting relative angle!" << std::endl;
         *     } else {
         *         std::cout << "Relative angle: " << angle.convert(deg) << std::endl;
         *     }
         * }
         * @endcode
         */
        virtual Angle getAngle() = 0;
        /**
         * @brief Set the relative angle of the encoder
         *
         * This function sets the relative angle of the encoder. The relative angle is the number of rotations the
         * encoder has measured since the last reset. This function is non-blocking.
         *
         * @param angle the relative angle to set the measured angle to
         * @return 0 on success
         * @return INT_MAX on failure, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     Encoder* encoder;
         *     if (encoder->setAngle(0_stDeg) == 0) {
         *         std::cout << "Relative angle set!" << std::endl;
         *         std::cout < "Relative angle: " << encoder->getAngle().convert(deg) << std::endl; // outputs 0
         *     } else {
         *         std::cout << "Error setting relative angle!" << std::endl;
         *     }
         * }
         * @endcode
         */
        virtual int setAngle(Angle angle) = 0;
        virtual ~Encoder() = default;
};
} // namespace lemlib