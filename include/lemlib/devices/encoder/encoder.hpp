#pragma once

#include "pros/rotation.hpp"
#include "pros/adi.hpp"
#include "pros/motors.hpp"

namespace lemlib {
class Encoder {
    public:
        /**
         * @brief Construct a new Encoder
         * 
         * @param pollRate int ms describing how often the sensor's data is updated (for v5 it should 10ms)
         */
        Encoder(const int pollRate)
            : pollRate(pollRate) {}

        /**
         * @brief Get the angle rotated by the encoder, in radians
         *
         * @return float angle rotated by the encoder, in radians
         */
        virtual float getAngle() = 0;

        /**
         * @brief Get the angle rotated by the encoder since the last time it was checked, in radians
         *
         * @param update whether to update the last angle measured by the encoder. True by default
         * @return float angle rotated by the encoder, in radians
         */
        float getAngleDelta(bool update = true);

        /**
         * @brief Reset the encoder
         *
         * @return true encoder calibration failed
         * @return false encoder calibration succeeded
         */
        virtual bool reset() = 0;

        /**
         * @brief Informs the Odometry task how often this sensor should be read
         * 
         * @warning should not be used by ordinary users
         * 
         * @return time, in ms, between sensor data updates
         */
        int getPollRate() const;
    protected:
        float lastAngle = 0;
        const int pollRate;
};
} // namespace lemlib
