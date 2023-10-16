#pragma once

namespace lemlib {
class Gyro {
    public:
        /**
         * @brief Calibrate the gyro
         *
         * @param blocking whether the function should block until calibration is complete
         * @return true calibration failed
         * @return false calibration succeeded
         */
        virtual bool calibrate(bool blocking = false) = 0;
        /**
         * @brief Get wether the gyro is calibrating or not
         *
         * @return true Gyro is calibrating
         * @return false Gyro is not calibrating
         */
        virtual bool isCalibrating() = 0;
        /**
         * @brief Get wether the gyro is calibrated or not
         *
         * @return true the gyro is calibrated
         * @return false the gyro is not calibrated
         */
        virtual bool isCalibrated() = 0;
        /**
         * @brief Get whether the gyro is connected or not
         *
         * @return true the gyro is connected
         * @return false the gyro is not connected
         */
        virtual bool isConnected() = 0;
        /**
         * @brief Get the heading of the gyro
         *
         * @note 0 is in the positive x direction, and heading increases counterclockwise
         *
         * @return float heading, in radians, locked from 0-2pi
         */
        virtual float getHeading() = 0;
        /**
         * @brief Get the orientation of the gyro
         *
         * @note 0 is in the positive x direction, and heading increases counterclockwise
         *
         * @return float orientation, in radians
         */
        virtual float getOrientation() = 0;
        /**
         * @brief Set the orientation of the gyro
         *
         * @brief 0 is in the positive x direction, and heading increases counterclockwise
         *
         * @param orientation orientation, in radians
         */
        virtual void setOrientation(float orientation) = 0;
};
} // namespace lemlib