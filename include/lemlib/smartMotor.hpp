#pragma once

#include <math.h>
#include "pros/imu.hpp"
#include "pros/motor_group.hpp"
#include "pros/motors.h"
#include "pros/rotation.hpp"
#include "pros/rtos.h"
#include "lemlib/logger/logger.hpp"
#include "lemlib/util.hpp"
#include "pros/rtos.hpp"

namespace lemlib {

/**
 * @brief A class for implementing motor PID control with multiple sensor options.
 * 
 * The SmartMotor class provides a convenient interface for controlling motors using PID feedback 
 * loops with various sensor configurations. It supports three types of inputs:
 * 1. 3-Wire Encoders (ADI Encoder)
 * 2. V5 Rotation Sensors
 * 3. Integrated Motor Encoders (IME)
 */
class SmartMotor {
    public:
        /**
         * @brief Constructor for SmartMotor using an ADI Encoder.
         * 
         * @param actuator Pointer to the motor group that will be controlled.
         * @param sensor Pointer to the ADI Encoder providing position feedback.
         * @param controller PID configuration for the control loop.
         */
        SmartMotor(pros::MotorGroup* actuator, pros::adi::Encoder* sensor, PID controller);

        /**
         * @brief Constructor for SmartMotor using a V5 Rotation Sensor.
         * 
         * @param actuator Pointer to the motor group that will be controlled.
         * @param sensor Pointer to the V5 Rotation Sensor providing position feedback.
         * @param controller PID configuration for the control loop.
         */
        SmartMotor(pros::MotorGroup* actuator, pros::Rotation* sensor, PID controller);

        /**
         * @brief Constructor for SmartMotor using an Integrated Motor Encoder (IME).
         * 
         * @param actuator Pointer to the motor group with built-in IME for position feedback.
         * @param controller PID configuration for the control loop.
         */
        SmartMotor(pros::MotorGroup* actuator, PID controller);

        /**
         * @brief Resets the rotation position of the motor.
         * 
         * This function resets the sensor or encoder position back to zero.
         * 
         * @return Always returns 0 (reserved for future functionality).
         */
        int reset();

        /**
         * @brief Retrieves the current rotation/position of the motor.
         * 
         * @return The motor's current position in sensor units as a float.
         */
        float getRotation();

        /**
         * @brief Moves the motor to a target position using PID control.
         * 
         * This function runs a PID loop to drive the motor to a specified target position. It can 
         * operate synchronously or asynchronously depending on the `async` flag.
         * 
         * @param target Target position for the motor in sensor units.
         * @param timeout Maximum time (in milliseconds) to reach the target position.
         * @param acceptableRange Allowable range within the target position to consider as "reached".
         * @param async If true, the function runs asynchronously in a separate task.
         * @return An integer status code:
         *         - 1: Target position successfully reached.
         *         - 0: Timeout occurred.
         *         - 2: Asynchronous task started.
         */
        int movePID(float target, float timeout, float acceptableRange = 0.5, bool async = false);


    private:
        PID controller;                   ///< PID configuration for the control loop.
        pros::MotorGroup* actuator;       ///< Pointer to the controlled motor group.

        pros::adi::Encoder* encoder = nullptr; ///< Pointer to the ADI Encoder sensor (if used).
        pros::Rotation* rotation = nullptr;    ///< Pointer to the V5 Rotation Sensor (if used).
        pros::MotorGroup* ime = nullptr;       ///< Pointer to the IME-based motor group (if used).
};
}