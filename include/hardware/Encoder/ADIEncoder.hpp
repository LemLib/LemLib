#pragma once

#include "hardware/Encoder/Encoder.hpp"
#include "hardware/Port.hpp"
#include "pros/adi.hpp"
#include "pros/rtos.hpp"

namespace lemlib {
/**
 * @brief Encoder implementation for the Optical Shaft Encoder
 *
 */
class ADIEncoder : public Encoder {
    public:
        /**
         * @brief Construct a new Optical Shaft Encoder
         *
         * @param encoder the pros::ADIEncoder object to use
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::ADIEncoder encoder = pros::adi::Encoder('A', 'B');
         * }
         * @endcode
         */
        ADIEncoder(pros::adi::Encoder encoder);
        /**
         * @brief Construct a new Optical Shaft Encoder
         *
         * @param ports the two ports for the optical shaft encoder (1-8, 'a'-'h', 'A'-'H')
         * @param reversed whether the encoder is reversed or not
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     // optical shaft encoder on ports 'a' and 'b', which is reversed
         *     lemlib::ADIEncoder encoder({'a', 'b'}, true);
         * }
         * @endcode
         */
        ADIEncoder(ADIPair ports, bool reversed);
        /**
         * @brief Construct a new Optical Shaft Encoder
         *
         * @param expanderPort the port of the ADI Expander
         * @param ports the two ports for the optical shaft encoder (1-8, 'a'-'h', 'A'-'H')
         * @param reversed whether the encoder is reversed or not
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     // optical shaft encoder on an ADI Expander
         *     // ADIExpander port: 2, top port: 'c', bottom port: 'd'
         *     // encoder is not reversed
         *     lemlib::ADIEncoder(2, {'c', 'd'}, false);
         * }
         * @endcode
         */
        ADIEncoder(SmartPort expanderPort, ADIPair ports, bool reversed);
        /**
         * @brief ADIEncoder copy constructor
         *
         * Because pros::Mutex does not have a copy constructor, an explicit
         * copy constructor for the ADIEncoder is necessary
         *
         * @param other the ADIEncoder to copy
         */
        ADIEncoder(const ADIEncoder& other);
        /**
         * @brief whether the encoder is connected
         *
         * @deprecated This function is deprecated because there is no way to check if the ADIEncoder is connected due
         * to the nature of the ADI ports. If this function is called, it will act as if the encoder is connected.
         * However, it may still return an error and set errno
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENODEV: the port could not be configured as an encoder
         *
         * @return 1 if there are no errors
         * @return INT_MAX if there is an error, setting errno
         */
        [[deprecated("This function is not implemented due to hardware limitations")]]
        int32_t isConnected() const override;
        /**
         * @brief Get the relative angle measured by the encoder
         *
         * The relative angle measured by the encoder is the angle of the encoder relative to the last time the encoder
         * was reset. As such, it is unbounded.
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENODEV: the port could not be configured as an encoder
         *
         * @return Angle the relative angle of the encoder
         * @return INFINITY if there is an error, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::ADIEncoder encoder = pros::adi::Encoder('A', 'B');
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
         * @brief Set the relative angle of the encoder
         *
         * This function sets the relative angle of the encoder. The relative angle is the number of rotations the
         * encoder has measured since the last reset. This function is non-blocking.
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENODEV: the port could not be configured as an encoder
         *
         * @param angle the angle to set the measured angle to
         * @return 0 on success
         * @return INT_MAX on failure, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::ADIEncoder encoder = pros::adi::Encoder('A', 'B');
         *     const int result = encoder.setAngle(0_stDeg);
         *     if (result == 0) {
         *         std::cout << "Relative angle set!" << std::endl;
         *         std::cout < "Relative angle: " << encoder.getAngle().convert(deg) << std::endl; // outputs 0
         *     } else {
         *         std::cout << "Error setting relative angle!" << std::endl;
         *     }
         * }
         * @endcode
         */
        int32_t setAngle(Angle angle) override;
    private:
        mutable pros::Mutex m_mutex;
        pros::adi::Encoder m_encoder;
        Angle m_offset = 0_stDeg;
};
} // namespace lemlib