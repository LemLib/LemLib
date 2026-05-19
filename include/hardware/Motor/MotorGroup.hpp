#pragma once

#include "hardware/Motor/Motor.hpp"
#include "hardware/Port.hpp"
#include "units/Angle.hpp"
#include "pros/motor_group.hpp"
#include "pros/rtos.hpp"
#include <vector>

namespace lemlib {
/**
 * @brief MotorGroup class
 *
 * This class is a handler for a group of lemlib::Motor objects, which themselves are wrappers for ther pros::Motor
 * objects. This class allows for easy control of collection of telemetry, as inputs and outputs are unitized. This
 * class also enables users to add and remove motors from the group, which is useful when a motor can be moved between
 * subsystems using a Power Take Off (PTO) or similar mechanism.
 *
 * Error handling for the MotorGroup class is a bit different from other hardware classes. This is because
 * the MotorGroup class represents a group of motors, any of which could fail. However, as long as one
 * motor in the group is functioning properly, the MotorGroup will not throw any errors. In addition, errno will be set
 * to whatever error was thrown last, as there may be multiple motors in a motor group.
 */
class MotorGroup : public Encoder {
    public:
        /**
         * @brief Construct a new Motor Group
         *
         * @param ports list of ports of the motors in the group
         * @param outputVelocity the theoretical maximum output velocity of the motor group, after gearing
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     // motor group with motors on ports 1, -2, and 3
         *     // max theoretical output is 360 rpm
         *     lemlib::MotorGroup motorGroup({1, -2, 3}, 360_rpm);
         * }
         * @endcode
         */
        MotorGroup(const std::initializer_list<ReversibleSmartPort>& ports, AngularVelocity outputVelocity);
        /**
         * @brief MotorGroup copy constructor
         *
         * Because pros::Mutex does not have a copy constructor, an explicit
         * copy constructor is necessary
         *
         * @param other the MotorGroup to copy
         */
        MotorGroup(const MotorGroup& other);
        /**
         * @brief Create a new Motor Group
         *
         * @param group the pros motor group to get the ports from
         * @param outputVelocity the theoretical maximum output velocity of the motor group, after gearing
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     // pros motor group with motors on ports 1, -2, and 3
         *     pros::MotorGroup prosMotorGroup({1, -2, 3});
         *     // motor group that uses the same motors as the pros motor group
         *     // and spins at 600 rpm
         *     lemlib::MotorGroup motorGroup(prosMotorGroup, 600_rpm);
         * }
         * @endcode
         */
        static MotorGroup from_pros_group(const pros::MotorGroup group, AngularVelocity outputVelocity);
        /**
         * @brief move the motors at a percent power from -1.0 to +1.0
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENODEV: the port cannot be configured as a motor
         *
         * @param percent the power to move the motors at from -1.0 to +1.0
         * @return 0 on success
         * @return INT_MAX on failure, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::MotorGroup motorGroup({1, -2, 3}, 360_rpm);
         *
         *     // move the motors forward at 50% power
         *     motorGroup.move(0.5);
         *     // move the motors backward at 50% power
         *     motorGroup.move(-0.5);
         *     // stop the motors
         *     motorGroup.move(0);
         * }
         * @endcode
         */
        int32_t move(Number percent);
        /**
         * @brief move the motors at a given angular velocity
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENODEV: the port cannot be configured as a motor
         *
         * @param velocity the target angular velocity to move the motors at
         * @return 0 on success
         * @return INT_MAX on failure, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::MotorGroup motorGroup({1, -2, 3}, 360_rpm);
         *
         *     // move the motors forward at 50 degrees per second
         *     motorGroup.moveVelocity(50_degps);
         *     // move the motors backward at 50 degrees per second
         *     motorGroup.moveVelocity(-50_degps);
         *     // stop the motors
         *     motorGroup.moveVelocity(0_degps);
         * }
         * @endcode
         */
        int32_t moveVelocity(AngularVelocity velocity);
        /**
         * @brief brake the motors
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENODEV: the port cannot be configured as a motor
         *
         * This function will stop the motors using the set brake mode
         *
         * @return 0 on success
         * @return INT_MAX on failure, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::MotorGroup motorGroup({1, -2, 3}, 360_rpm);
         *
         *     // move the motors forward at 50% power
         *     motorGroup.move(0.5);
         *     // brake the motors
         *     motorGroup.brake();
         * }
         * @endcode
         */
        int32_t brake();
        /**
         * @brief set the brake mode of the motors
         *
         * @param mode the brake mode to set the motors to
         * @return 0 on success
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::MotorGroup motorGroup({1, -2, 3}, 360_rpm);
         *
         *     // set the motors to brake when stopped
         *     motorGroup.setBrakeMode(lemlib::BrakeMode::BRAKE);
         *     // set the motors to coast when stopped
         *     motorGroup.setBrakeMode(lemlib::BrakeMode::COAST);
         *     // set the motors to hold when stopped
         *     motorGroup.setBrakeMode(lemlib::BrakeMode::HOLD);
         * }
         * @endcode
         */
        int32_t setBrakeMode(BrakeMode mode);
        /**
         * @brief get the brake mode of the motor group
         *
         * @return BrakeMode enum value of the brake mode
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::MotorGroup motorGroup({1, -2, 3}, 360_rpm);
         *
         *     const lemlib::BrakeMode mode = motorGroup.getBrakeMode();
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
         * @brief whether any of the motors in the motor group are connected
         *
         * @return 0 no motors are connected
         * @return 1 if at least one motor is connected
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::MotorGroup motorGroup({1, -2, 3}, 360_rpm);
         *
         *     const int result = motorGroup.isConnected();
         *     if (result == 1) {
         *         std::cout << "Encoder is connected!" << std::endl;
         *     } else if (result == 0) {
         *         std::cout << "Encoder is not connected!" << std::endl;
         *     } else {
         *         std::cout << "Error checking if encoder is connected!" << std::endl;
         *     }
         * }
         * @endcode
         */
        int32_t isConnected() const override;
        /**
         * @brief Get the average relative angle measured by the motors
         *
         * The relative angle measured by the encoder is the angle of the encoder relative to the last time the encoder
         * was reset. As such, it is unbounded.
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENODEV: the port cannot be configured as a motor
         *
         * @return Angle the relative angle measured by the encoder
         * @return INFINITY if there is an error, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::MotorGroup motorGroup({1, -2, 3}, 360_rpm);
         *
         *     const Angle angle = motorGroup.getAngle();
         *     if (angle == INFINITY) {
         *         std::cout << "Error getting relative angle!" << std::endl;
         *     } else {
         *         std::cout << "Relative angle: " << to_sDeg(angle) << std::endl;
         *     }
         * }
         * @endcode
         */
        Angle getAngle() const override;
        /**
         * @brief Set the relative angle of all the motors
         *
         * This function sets the relative angle of the encoder. The relative angle is the number of rotations the
         * encoder has measured since the last reset. This function is non-blocking.
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENODEV: the port cannot be configured as a motor
         *
         * @param angle the relative angle to set the measured angle to
         * @return 0 on success
         * @return INT_MAX on failure, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::MotorGroup motorGroup({1, -2, 3}, 360_rpm);
         *
         *     if (motorGroup.setAngle(0_stDeg) == 0) {
         *         std::cout << "Relative angle set!" << std::endl;
         *         std::cout < "Relative angle: " << motorGroup.getAngle().convert(deg) << std::endl; // outputs 0
         *     } else {
         *         std::cout << "Error setting relative angle!" << std::endl;
         *     }
         * }
         * @endcode
         */
        int32_t setAngle(Angle angle) override;
        /**
         * @brief Get the combined current limit of all motors in the group
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * TODO: see how overheating affects this value
         *
         * ENODEV: the port cannot be configured as a motor
         *
         * @return Current the combined current limit of the motor group
         * @return INFINITY on failure, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::MotorGroup motorGroup({1, -2, 3}, 360_rpm);
         *
         *     // output the current limit to the console
         *     Current limit = motorGroup.getCurrentLimit();
         *     if (units::to_amp(limit) == INFINITY) {
         *         std::cout << "Error getting motor group current limit" << std::endl;
         *     } else {
         *         std::cout << "Current Limit: " << units::to_amp(limit) << std::endl;
         *     }
         * }
         * @endcode
         */
        Current getCurrentLimit() const;
        /**
         * @brief set the combined current limit of all motors in the group
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
         *     lemlib::MotorGroup motorGroup({1, -2, 3}, 360_rpm);
         *     // set the current limit to 6 amp
         *     // every motor in the group will have a current limit of 2 amp
         *     // making for a total current limit of 6 amp
         *     motorGroup.setCurrentLimit(6_amp);
         * }
         * @endcode
         */
        int32_t setCurrentLimit(Current limit);
        /**
         * @brief Get the temperatures of the motors in the motor group
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENODEV: the port cannot be configured as a motor
         *
         * @return vector<Temperature> vector of the temperatures of the motors
         * @return INFINITY on failure, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::MotorGroup motorGroup({1, -2, 3}, 360_rpm);
         *
         *     // output motor temperatures to the console
         *     std::vector<Temperature> temperatures = motorGroup.getTemperatures();
         *     for (lemlib::Motor motor : motorGroup) {
         *         if (units::to_celsius(temperature) == INFINITY) {
         *             std::cout << "Error getting motor temperature" << std::endl;
         *         } else {
         *             std::cout << "Motor Temperature: " << units::to_celsius(motor.getTemperature()) << std::endl;
         *         }
         *     }
         * }
         * @endcode
         */
        std::vector<Temperature> getTemperatures() const;
        /**
         * @brief set the output velocity of the motors
         *
         * @param outputVelocity the theoretical maximum output velocity of the motor group, after gearing, to set
         * @return int 0 success
         * @return INT_MAX error occurred, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::MotorGroup motorGroup({1, -2, 3}, 360_rpm);
         *     // set the output velocity to 450 rpm
         *     motorGroup.setOutputVelocity(450_rpm);
         * }
         * @endcode
         */
        int32_t setOutputVelocity(AngularVelocity outputVelocity);
        /**
         * @brief Get the output velocity of the motor group
         *
         * @return AngularVelocity the output velocity of the motor group
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::MotorGroup motorGroup({1, -2, 3}, 360_rpm);
         *     std::cout << "output velocity: " << motorGroup.getOutputVelocity() << std::endl; // outputs 360 rpm
         * }
         * @endcode
         */
        AngularVelocity getOutputVelocity() const;
        /**
         * @brief Get the number of connected motors in the group
         *
         * @return int the number of connected motors in the group
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::MotorGroup motorGroup({1, -2, 3}, 360_rpm);
         *
         *     std::cout << "Number of connected motors: " << motorGroup.getSize() << std::endl;
         * }
         * @endcode
         */
        int32_t getSize() const;
        /**
         * @brief Add a motor to the motor group
         *
         * This function adds a motor to the motor group. If successful, it will set the angle measured by the motor to
         * the average angle measured by the motor group. It will also set the brake mode of the motor to that of the
         * first working motor in the group. If there are any errors, the motor will still be added to the group and it
         * will be configured as soon as it is functional again.
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENODEV: the port cannot be configured as a motor
         *
         * @param port the signed port of the motor to be added to the group. Negative ports indicate the motor should
         * be reversed
         *
         * @return 0 on success
         * @return INT_MAX on failure, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::MotorGroup motorGroup({1, -2, 3}, 360_rpm);
         *
         *     // add a motor to the group
         *     motorGroup.addMotor(4);
         * }
         * @endcode
         */
        int32_t addMotor(ReversibleSmartPort port);
        /**
         * @brief Add a motor to the motor group
         *
         * This function adds a motor to the motor group. If successful, it will set the angle measured by the motor to
         * the average angle measured by the motor group. It will also set the brake mode of the motor to that of the
         * first working motor in the group. If there are any errors, the motor will still be added to the group and it
         * will be configured as soon as it is functional again.
         *
         * @param motor the motor to be added to the group
         *
         * @return 0 on success
         * @return INT_MAX on failure, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::MotorGroup motorGroup({1, -2, 3}, 360_rpm);
         *
         *     // add a motor to the group
         *     pros::Motor motor4(4, pros::v5::MotorGears::green);
         *     motorGroup.addMotor(motor4);
         * }
         * @endcode
         */
        int32_t addMotor(Motor motor);
        /**
         * @brief Add a motor to the motor group
         *
         * This function adds a motor to the motor group. If successful, it will set the angle measured by the motor to
         * the average angle measured by the motor group. It will also set the brake mode of the motor to that of the
         * first working motor in the group. If there are any errors, the motor will still be added to the group and it
         * will be configured as soon as it is functional again.
         *
         * @param motor the motor to be added to the group
         * @param reversed whether the motor should be reversed
         *
         * @return 0 on success
         * @return INT_MAX on failure, setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::MotorGroup motorGroup({1, -2, 3}, 360_rpm);
         *
         *    // add a motor to the group, which should be reversed
         *     pros::Motor motor4(4, pros::v5::MotorGears::green);
         *     motorGroup.addMotor(motor4, true);
         * }
         * @endcode
         */
        int32_t addMotor(Motor motor, bool reversed);
        /**
         * @brief Remove a motor from the motor group
         *
         * @param port the port of the motor to be removed from the group
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::MotorGroup motorGroup({1, -2, 3}, 360_rpm);
         *
         *     // remove a motor from the group
         *     motorGroup.removeMotor(4);
         * }
         * @endcode
         */
        void removeMotor(ReversibleSmartPort port);
        /**
         * @brief Remove a motor from the motor group
         *
         * @param motor the motor to be removed from the group
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *     lemlib::MotorGroup motorGroup({1, -2, 3}, 360_rpm);
         *
         *     // remove a motor from the group
         *     pros::Motor motor4(4, pros::v5::MotorGears::green);
         *     motorGroup.removeMotor(motor4);
         * }
         * @endcode
         */
        void removeMotor(Motor motor);
    private:
        struct MotorInfo {
                ReversibleSmartPort port;
                bool connectedLastCycle;
                Angle offset;
        };

        /**
         * @brief Configure a motor so its ready to join the motor group, and return its offset
         *
         * Motors may be added to the motor group or reconnect to the motor group during runtime. When this happens,
         * functions like getAngle() would break as the motor is not configured like other motors in the group. This
         * function sets the angle measured by the specified motor to the average angle measured by the group. It also
         * sets the brake mode of the motor to be the same as the first working motor in the group.
         *
         * This function uses the following values of errno when an error state is reached:
         *
         * ENODEV: the port cannot be configured as a motor
         *
         * @param port the port of the motor to configure
         *
         * @return 0 on success
         * @return INT_MAX on failure, setting errno
         */
        Angle configureMotor(ReversibleSmartPort port) const;
        /**
         * @brief Get motors in the motor group as a vector of lemlib::Motor objects
         *
         * This function exists to simplify logic in the MotorGroup source code.
         *
         * @return const std::vector<Motor> vector of lemlib::Motor objects
         */
        const std::vector<Motor> getMotors() const;
        /**
         * @brief Get the Motor Infos
         *
         * This function exists to make the copy constructor thread-safe
         *
         * @return const std::vector<MotorInfo>
         */
        const std::vector<MotorInfo> getMotorInfo() const;

        mutable pros::Mutex m_mutex;
        BrakeMode m_brakeMode = BrakeMode::COAST;
        AngularVelocity m_outputVelocity;
        /**
         * This member variable is a vector of motor information
         *
         * Ideally, we'd use a vector of lemlib::Motor objects, but this does not work if you want to remove an element
         * from the vector as the copy constructor is implicitly deleted.
         *
         * The ports are signed to indicate whether a motor should be reversed or not.
         *
         * It also has a bool for every port, which represents whether the motor was connected or not the last time
         * `getAngle` was called. This enables the motor group to properly handle a motor reconnect.
         *
         * It also contains the offset of each motor. This needs to be saved by the motor group, because it can't be
         * saved in a motor object, as motor objects are not saved as member variables
         */
        mutable std::vector<MotorInfo> m_motors;
};
}; // namespace lemlib
