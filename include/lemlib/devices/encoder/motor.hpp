#pragma once

#include <memory>
#include "pros/motor_group.hpp"
#include "lemlib/devices/encoder/encoder.hpp"

namespace lemlib {
class MotorEncoder : public Encoder {
    public:
        /**
         * @brief Construct a new Motor Encoder
         *
         * @note change to reference instead of pointer when PROS 4 releases
         *
         * @param motors pointer to the motor group to be used
         * @param rpm output rpm
         */
        MotorEncoder(std::unique_ptr<pros::MotorGroup>&& motors, float rpm);

        /**
         * @brief Get the angle rotated by the motor encoders, in radians
         *
         * @return float angle rotated by the motor encoders, in radians
         */
        float getAngle() const override;
        /**
         * @brief Reset the motor encoder
         *
         * @return true calibration failed
         * @return false calibration succeeded
         */
        bool reset() const override;
    private:
        std::unique_ptr<pros::MotorGroup> motors;
        const float rpm;
};
} // namespace lemlib
