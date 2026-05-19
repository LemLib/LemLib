#pragma once

#include "hardware/Encoder/Encoder.hpp"
#include "hardware/Port.hpp"
#include "units/Temperature.hpp"
#include "pros/rtos.hpp"
#include "pros/motors.hpp"

namespace lemlib {

enum class BrakeMode { COAST, BRAKE, HOLD, INVALID };

enum class MotorType { V5, EXP, INVALID };

class Motor : public Encoder {
    public:
        /**
         * @brief Construct a new Motor object
         *
         * @param port the signed port of the motor. Negative if the motor is reversed
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     // construct a new Motor object on port 1, which is reversed, and powers a mechanism that spins at 15 rpm
         *     lemlib::Motor motor(-1, 15_rpm);
         * }
         * @endcode
         */
        Motor(ReversibleSmartPort port, AngularVelocity outputVelocity);
        /**
         * @brief Motor copy constructor
         *
         * Because pros::Mutex does not have a copy constructor, an explicit
         * copy constructor is necessary
         *
         * @param other the Motor to copy
         */
        Motor(const Motor& other);
        /**
         * @brief Create a new Motor object
         *
         * @param motor the pros motor object to get the port from
         * @param outputVelocity the maximum theoretical velocity of the motor
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     // create a pros motor on port 1, which is reversed
         *     pros::Motor prosMotor(-1);
         *     // create a motor which is reversed, on port 1, and spins at 200 rpm
         *     lemlib::Motor motor = lemlib::Motor::from_pros_motor(prosMotor, 200_rpm);
         * }
         * @endcode
         */
        static Motor from_pros_motor(const pros::Motor motor, AngularVelocity outputVelocity);
        /**
         * @brief move the motor at a percent power from -1.0 to +1.0
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENODEV: the port cannot be configured as a motor
         *
         * @param percent the power to move the motor at from -1.0 to +1.0
         * @return 0 on success
         * @return INT_MAX on failure, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::Motor motor(1, 200_rpm);
         *     // move the motor forward at 50% power
         *     motor.move(0.5);
         *     // move the motor backward at 50% power
         *     motor.move(-0.5);
         *     // stop the motor
         *     motor.move(0);
         * }
         * @endcode
         */
        int32_t move(Number percent);
        /**
         * @brief move the motor at a given angular velocity
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENODEV: the port cannot be configured as a motor
         *
         * @param velocity the target angular velocity to move the motor at
         * @return 0 on success
         * @return INT_MAX on failure, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::Motor motor(1, 200_rpm);
         *     // move the motor forward at 50 degrees per second
         *     motor.moveVelocity(50_degps);
         *     // move the motor backward at 50 degrees per second
         *     motor.moveVelocity(-50_degps);
         *     // stop the motor
         *     motor.moveVelocity(0_degps);
         * }
         * @endcode
         */
        int32_t moveVelocity(AngularVelocity velocity);
        /**
         * @brief brake the motor
         *
         * This function will stop the motor using the set brake mode
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENODEV: the port cannot be configured as a motor
         *
         * @return 0 on success
         * @return INT_MAX on failure, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::Motor motor(1, 200_rpm);
         *     // move the motor forward at 50% power
         *     motor.move(0.5);
         *     // brake the motor
         *     motor.brake();
         * }
         * @endcode
         */
        int32_t brake();
        /**
         * @brief set the brake mode of the motor
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENODEV: the port cannot be configured as a motor
         * EINVAL: can't set brake mode to an invalid state
         *
         * @param mode the brake mode to set the motor to
         * @return 0 on success
         * @return INT_MAX on failure, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::Motor motor(1, 200_rpm);
         *     // set the motor to brake when stopped
         *     motor.setBrakeMode(lemlib::BrakeMode::BRAKE);
         *     // set the motor to coast when stopped
         *     motor.setBrakeMode(lemlib::BrakeMode::COAST);
         *     // set the motor to hold when stopped
         *     motor.setBrakeMode(lemlib::BrakeMode::HOLD);
         * }
         * @endcode
         */
        int32_t setBrakeMode(BrakeMode mode);
        /**
         * @brief get the brake mode of the motor
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENODEV: the port cannot be configured as a motor
         *
         * @return BrakeMode enum value of the brake mode
         * @return BrakeMode::INVALID on failure, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::Motor motor(1, 200_rpm);
         *     const lemlib::BrakeMode mode = motor.getBrakeMode();
         *     if (mode == lemlib::BrakeMode::BRAKE) {
         *         std::cout << "Brake mode is set to BRAKE!" << std::endl;
         *     } else if (mode == lemlib::BrakeMode::COAST) {
         *         std::cout << "Brake mode is set to COAST!" << std::endl;
         *     } else if (mode == lemlib::BrakeMode::HOLD) {
         *         std::cout << "Brake mode is set to HOLD!" << std::endl;
         *     } else {
         *         std::cout << "Error getting brake mode!" << std::endl;
         *     }
         * }
         * @endcode
         */
        BrakeMode getBrakeMode() const;
        /**
         * @brief whether the motor is connected
         *
         * @return 0 if its not connected
         * @return 1 if it is connected
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::Motor motor(1, 200_rpm);
         *     const int result = motor.isConnected();
         *     if (result == 1) {
         *         std::cout << "motor is connected!" << std::endl;
         *     } else if (result == 0) {
         *         std::cout << "motor is not connected!" << std::endl;
         *     } else {
         *         std::cout << "Error checking if motor is connected!" << std::endl;
         *     }
         * }
         * @endcode
         */
        int32_t isConnected() const override;
        /**
         * @brief Get the relative angle measured by the motor
         *
         * The relative angle measured by the motor is the angle of the motor relative to the last time the motor
         * was reset. As such, it is unbounded.
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENODEV: the port cannot be configured as a motor
         *
         * @return Angle the relative angle measured by the motor
         * @return INFINITY if there is an error, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::Motor motor(1, 200_rpm);
         *     const Angle angle = motor.getAngle();
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
         * @brief Set the relative angle of the motor
         *
         * Setting the relative angle of the motor does so only on a software level, meaning that any other
         * lemlib::Motor objects will not register a change in the angle, only this object will
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENODEV: the port cannot be configured as a motor
         *
         * This function sets the relative angle of the motor. The relative angle is the number of rotations the
         * motor has measured since the last reset. This function is non-blocking.
         *
         * @param angle the relative angle to set the measured angle to
         * @return 0 on success
         * @return INT_MAX on failure, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::Motor motor(1, 200_rpm);
         *     if (motor.setAngle(0_stDeg) == 0) {
         *         std::cout << "Relative angle set!" << std::endl;
         *         std::cout < "Relative angle: " << to_sDeg(motor.getAngle()) << std::endl; // outputs 0
         *     } else {
         *         std::cout << "Error setting relative angle!" << std::endl;
         *     }
         * }
         * @endcode
         */
        int32_t setAngle(Angle angle) override;
        /**
         * @brief Get the offset of the motor encoder
         *
         * Motor position is calculated as raw position + offset
         *
         * @return Angle
         *
         * @code {.cpp}
         * void initialize() {
         *     lemlib::Motor motor(1, 200_rpm);
         *
         *     // expected output: 0 degrees
         *     std::cout << "offset: " << to_stDeg(motor.getOffset()) << std::endl;
         *     // expected output: 0 degrees
         *     std::cout << "angle: " << to_stDeg(motor.getAngle()) << std::endl;
         *
         *     // set the motor angle
         *     motor.setAngle(180_stDeg);
         *
         *     // expected output: 180 degrees
         *     std::cout << "offset: " << to_stDeg(motor.getOffset()) << std::endl;
         *     // expected output: 180 degrees
         *     std::cout << "angle: " << to_stDeg(motor.getAngle()) << std::endl;
         * }
         * @endcode
         */
        Angle getOffset() const;
        /**
         * @brief Set the offset of the motor encoder
         *
         * Motor position is calculated as raw position + offset. This function sets the offset
         *
         * @param offset the new offset
         * @return int 0 on success
         *
         * @b Example
         * @code {.cpp}
         * void initialize() {
         *     lemlib::Motor motor(1, 200_rpm);
         *
         *     // expected output: 0 degrees
         *     std::cout << "angle: " << to_stDeg(motor.getAngle()) << std::endl;
         *
         *     // set the motor offset
         *     motor.setOffset(180_stDeg);
         *     // expected output: 180
         *     std::cout << "angle: " << to_stDeg(motor.getAngle()) << std::endl;
         * }
         * @endcode
         */
        int32_t setOffset(Angle offset);
        /**
         * @brief Get the type of the motor
         *
         * There are 2 motors legal for use: The 11W V5 motor and the 5.5W EXP motor
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENODEV: the port cannot be configured as a motor
         *
         * @return MotorType the type of the motor
         * @return MotorType::Invalid on failure, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::Motor motor(1, 200_rpm);
         *     switch (motor.getType()) {
         *        case (lemlib::MotorType::V5): {
         *            std::cout << "V5 Motor on port 1" << std::endl;
         *            break;
         *        }
         *        case (lemlib::MotorType::EXP): {
         *            std::cout << "EXP Motor on port 1" << std::endl;
         *            break;
         *        }
         *        default: std::cout << "Error getting type of motor on port 1" << std::endl;
         *     }
         * }
         * @endcode
         */
        MotorType getType() const;
        /**
         * @brief get whether the motor is reversed
         *
         * @return 0 if the motor is not reversed
         * @return 1 if the motor is reversed
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::Motor motor(1, 200_rpm);
         *     if (motor.isReversed() == 1) {
         *         std::cout << "Motor is reversed!" << std::endl;
         *     } else if (motor.isReversed() == 0) {
         *         std::cout << "Motor is not reversed!" << std::endl;
         *     } else {
         *         std::cout << "Error checking if motor is reversed!" << std::endl;
         *     }
         * }
         * @endcode
         */
        int32_t isReversed() const;
        /**
         * @brief set whether the motor should be reversed or not
         *
         * @param reversed whether the motor should be reversed or not
         * @return 0 on success
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::Motor motor(1, 200_rpm);
         *     // reverse the motor
         *     motor.setReversed(true);
         * }
         * @endcode
         */
        int32_t setReversed(bool reversed);
        /**
         * @brief Get the port the motor is connected to
         *
         * Just like in PROS, the port is negative if the motor is reversed
         *
         * @return int the port the motor is connected to
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::Motor motor(1, 200_rpm);
         *     std::cout << "Motor is connected to port " << motor.getPort() << std::endl;
         * }
         * @endcode
         */
        ReversibleSmartPort getPort() const;
        /**
         * @brief Get the current limit of the motor
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * TODO: see how overheating affects this value
         *
         * ENODEV: the port cannot be configured as a motor
         *
         * @return Current the current limit of the motor
         * @return INFINITY on failure, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::Motor motor(1, 200_rpm);
         *
         *     // output the current limit to the console
         *     Current limit = motor.getCurrentLimit();
         *     if (units::to_amp(limit) == INFINITY) {
         *         std::cout << "Error getting motor current limit" << std::endl;
         *     } else {
         *         std::cout << "Current Limit: " << units::to_amp(limit) << std::endl;
         *     }
         * }
         * @endcode
         */
        Current getCurrentLimit() const;
        /**
         * @brief set the current limit of the motor
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENODEV: the port cannot be configured as a motor
         *
         * @param limit the maximum allowed current
         * @return 0 on success
         * @return INT_MAX on failure, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::Motor motor(1, 200_rpm);
         *     // set the current limit to 0.5 amp
         *     motor.setCurrentLimit(0.5_amp);
         * }
         * @endcode
         */
        int32_t setCurrentLimit(Current limit);
        /**
         * @brief Get the temperature of the motor
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENODEV: the port cannot be configured as a motor
         *
         * @return Temperature the temperature of the motor
         * @return INFINITY on failure, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::Motor motor(1, 200_rpm);
         *
         *     // output motor temperature to the console
         *     Temperature temperature = motor.getTemperature();
         *     if (units::to_celsius(temperature) == INFINITY) {
         *         std::cout << "Error getting motor temperature" << std::endl;
         *     } else {
         *         std::cout << "Motor Temperature: " << units::to_celsius(motor.getTemperature()) << std::endl;
         *     }
         * }
         * @endcode
         */
        Temperature getTemperature() const;
        /**
         * @brief set the output velocity of the motor
         *
         * @param outputVelocity the theoretical maximum output velocity of the motor, after gearing, to set
         * @return int 0 success
         * @return INT_MAX error occurred, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::Motor motor(1, 360_rpm);
         *     // set the output velocity to 450 rpm
         *     motor.setOutputVelocity(450_rpm);
         * }
         * @endcode
         */
        int32_t setOutputVelocity(AngularVelocity outputVelocity);
        /**
         * @brief Get the output velocity of the motor
         *
         * @return AngularVelocity
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::Motor motor(1, 360_rpm);
         *     // get the motor output
         *     std::cout << motor.getOutputVelocity() << std::endl; // output: 360_rpm
         * }
         * @endcode
         */
        AngularVelocity getOutputVelocity() const;
    private:
        mutable pros::Mutex m_mutex;
        AngularVelocity m_outputVelocity;
        Angle m_offset = 0_stDeg;
        ReversibleSmartPort m_port;
};
} // namespace lemlib