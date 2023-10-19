#pragma once

#include "pros/adi.hpp"
#include "lemlib/devices/encoder/encoder.hpp"

namespace lemlib {
class OpticalEncoder : public Encoder {
    public:
        /**
         * @brief Construct a new Optical Encoder
         *
         * @param topPort the top port of the optical encoder. 'a' to 'h'
         * @param bottomPort the bottom of the optical encoder 'a' to 'h'
         * @param reversed true if the optical encoder should be reversed, false otherwise
         * @param ratio the gear ratio to use. Input / Output
         */
        OpticalEncoder(char topPort, char bottomPort, bool reversed, float ratio);

        /**
         * @brief Get the angle rotated by the optical encoder, in radians
         *
         * @return float angle rotated by the optical encoder, in radians
         */
        float getAngle() override;
        /**
         * @brief Reset the optical encoder
         *
         * @return true calibration failed
         * @return false calibration succeeded
         */
        bool reset() override;
    private:
        pros::adi::Encoder optical;
        const float ratio;
};
} // namespace lemlib
