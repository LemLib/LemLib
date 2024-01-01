/**
 * @file include/lemlib/chassis/trackingWheel.hpp
 * @author LemLib Team
 * @brief tracking wheel class declarations
 * @version 0.4.5
 * @date 2023-01-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <memory>
#include <cstdint>
#include "pros/motor_group.hpp"
#include "pros/adi.hpp"
#include "pros/rotation.hpp"
#include "lemlib/devices/encoder/encoder.hpp"

namespace lemlib {

/**
 * @brief A namespace representing the size of omniwheels.
 */
namespace Omniwheel {
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
         * @brief Create a tracking wheel with a custom encoder
         *
         * @param encoder unique ptr to the custom encoder to be used
         * @param diameter the diameter of the wheel, in inches
         * @param offset distance between the wheel and the tracking center, in inches
         */
        TrackingWheel(std::shared_ptr<Encoder> encoder, float diameter, float offset);
        /**
         * @brief Create a new motor-encoder tracking wheel
         *
         * @param motors pointer to the motor group to be used for the tracking wheel
         * @param diameter the diameter of the wheel, in inches
         * @param offset distance between the wheel and the tracking center, in inches
         * @param rpm of the rpm of the wheels the motor group is driving
         */
        TrackingWheel(std::shared_ptr<pros::MotorGroup> motors, float diameter, float offset, float rpm);
        /**
         * @brief Create a new optical encoder tracking wheel
         *
         * @param topPort the top port of the optical shaft encoder. 'a' - 'h'
         * @param bottomPort the bottom port of the optical shaft encoder 'a' - 'h'
         * @param reversed whether the optical shaft encoder should be reversed or not
         * @param diameter the diameter of the wheel, in inches
         * @param offset distance between the wheel and the tracking center, in inches
         * @param ratio gear ratio of the tracking wheel, defaults to 1. Input / Output
         */
        TrackingWheel(char topPort, char bottomPort, bool reversed, float diameter, float offset, float ratio = 1);
        /**
         * @brief Create a new rotation sensor tracking wheel
         *
         * @param port the port the rotation sensor is connected to. 1-21
         * @param reversed whether the rotation sensor should be reversed or not
         * @param diameter the diameter of the wheel, in inches
         * @param offset distance between the wheel and the tracking center, in inches
         * @param ratio gear ratio of the tracking wheel, defaults to 1. Input / Output
         */
        TrackingWheel(uint8_t port, bool reversed, float diameter, float offset, float ratio = 1);
        /**
         * @brief Create a new rotation sensor tracking wheel
         *
         * @param port the signed port the rotation sensor is connected to. Positive is not reversed, negative is
         * reversed. 1-21
         * @param diameter the diameter of the wheel, in inches
         * @param offset distance between the wheel and the tracking center, in inches
         * @param ratio gear ratio of the tracking wheel, defaults to 1. Input / Output
         */
        TrackingWheel(int port, float diameter, float offset, float ratio = 1);

        /**
         * @brief Reset the tracking wheel position to 0
         *
         * @return true reset failed
         * @return false reset succeeded
         */
        bool reset();
        /**
         * @brief Get the distance traveled by the tracking wheel
         *
         * @return float distance traveled in inches
         */
        float getDistance();
        /**
         * @brief Get the difference between the current distance measured and the last distance measured
         *
         * @param update whether to update the last saved position. True by default
         * @return float difference in distance, in inches
         */
        float getDistanceDelta(bool update = true);
        /**
         * @brief Get the offset of the tracking wheel from the center of rotation
         *
         * @return float offset in inches
         */
        float getOffset() const;
        /**
         * @brief Get the diameter of the wheel
         *
         * @return float diameter, in inches
         */
        float getDiameter() const;
    private:
        std::shared_ptr<Encoder> encoder;
        float diameter;
        float offset;
};
} // namespace lemlib