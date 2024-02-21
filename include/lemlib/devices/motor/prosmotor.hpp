#pragma once

#include "lemlib/pid.hpp"
#include "lemlib/devices/motor/abstractmotor.hpp"
#include "pros/motors.h"
#include <memory>

namespace lemlib {

class PROSMotor : public Abstract_Motor {
    private:
        std::unique_ptr<pros::Motor> motor;
        std::shared_ptr<PROSMotor> pairMotor;

        std::shared_ptr<FAPID> pid;

        static constexpr int overheatTempCelsius = 55;

        bool isBroken = false;
        bool isReversed = false;
        bool hasPairMotor = false;

        int voltage = 0;

        float gearRatio;

        pros::motor_gearset_e gearset = pros::E_MOTOR_GEAR_GREEN;
    public:
        /**
         * @brief Construct a new Uvuv Motor object
         *
         * @param port V5 Smart Port number
         * @param isReversedArg Defines direction of the motor spinning. True is reversed, false is normal.
         * @param gearRatioArg Gear ratio of the motors mechanism. 3:4 = .75
         * @param gearset The PROS motor gearset.
         * @param pairMotorArg The motor paired with this motor. For example, leftFront and rightFront motors.
         * @param pid Built-in PID Controller for the motor.
         */
        PROSMotor(const uint8_t port, const bool isReversed, const float gearRatio,
                  const pros::v5::MotorGears gearset = pros::v5::MotorGears::rpm_600,
                  std::shared_ptr<PROSMotor> pairMotor = nullptr, std::shared_ptr<FAPID> pid = nullptr);

        void spinAtVoltage(int voltage) override;

        void spinPerc(int percent) override;

        void spinJoystick(int joystickValue) override;

        void spinPercVEXPID(int percent) override;

        void spinAtRPM(int RPM) override;

        void spinUntilDegree(int degree, int speedInVoltage) override;

        void spinFor(float seconds, int speedInVoltage) override;

        void logMotorPerformance() override; // Stuff like wattage and temperature, RPM, etc, along with Port Number

        void set_zero_position(int position) override;

        void shutDown() override;

        void revive() override;

        bool isOverheated() override;

        bool getIsBroken();

        float getRPM();

        float getVoltage();

        float getEncoderPos();
};

} // namespace lemlib