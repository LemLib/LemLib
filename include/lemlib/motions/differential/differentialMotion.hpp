#pragma once

#include "units/Pose.hpp"
#include "lemlib/chassis/differential/differentialSteerRequest.hpp"

namespace lemlib {

/**
 * @class DifferentialMotion
 *
 * @brief Abstract class which represents a motion algorithm for a differential drive robot
 *
 * We use this abstraction to allow for different motion algorithms to be used with the same interface.
 * This allows us to easily create and maintain motion algorithms while also allowing the user to
 * make custom motion algorithms without having to recompile the library.
 */
class DifferentialMotion {
    public:
        /**
         * @brief Calculates the target state of the chassis
         *
         * @param pose the current pose of the robot
         * @return std::shared_ptr<DifferentialSteerRequest> shared pointer to a implementation of
         * DifferentialSteerRequest
         */
        virtual std::shared_ptr<DifferentialSteerRequest> calculate(units::Pose pose) = 0;
        /**
         * @brief Get whether the motion is running
         *
         * @return true the motion is running
         * @return false the motion is not running
         */
        virtual bool isRunning();
        /**
         * @brief Destroy the Differential Motion object
         *
         */
        virtual ~DifferentialMotion();
    protected:
        bool running = true; /** whether the motion is running or not */
};
} // namespace lemlib