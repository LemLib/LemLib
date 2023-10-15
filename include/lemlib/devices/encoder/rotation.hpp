#pragma once

#include "pros/rotation.hpp"
#include "lemlib/devices/encoder/encoder.hpp"

namespace lemlib {
class RotationEncoder : public Encoder {
    public:
        /**
         * @brief Construct a new Rotation Encoder
         *
         * @param port the port of the rotation sensor
         * @param reversed true if the rotation sensor should be reversed, false otherwise
         * @param ratio the gear ratio to use. Input / Output
         */
        RotationEncoder(int port, bool reversed, float ratio);

        /**
         * @brief Get the angle rotated by the rotation sensor, in radians
         *
         * @return float angle rotated by the rotation sensor, in radians
         */
        float getAngle() override;
        /**
         * @brief Reset the rotation sensor
         *
         * @return true calibration failed
         * @return false calibration succeeded
         */
        bool reset() override;
    private:
        pros::Rotation rotation;
        const float ratio;
};
} // namespace lemlib
