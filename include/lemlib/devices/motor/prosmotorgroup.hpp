#pragma once

#include "prosmotor.hpp"
#include <memory>

namespace lemlib {

class PROSMotorGroup : public Abstract_Motor {
    private:
        std::vector<std::unique_ptr<PROSMotor>> motorContainer;
    public:
        PROSMotorGroup(std::vector<std::unique_ptr<PROSMotor>> motorContainerArg);

        PROSMotorGroup(std::vector<std::pair<MotorInfo, const pros::v5::MotorGears>> motorParameters);

        void spinAtVoltage(int voltage) override;

        void spinPerc(int percent) override;

        void spinJoystick(int joystickValue) override;

        void spinAtRPM(int RPM) override;

        void spinUntilDegree(int degree, int speedInVoltage) override;

        void spinFor(float seconds, int speedInVoltage) override;

        void logMotorPerformance() override; // Stuff like wattage and temperature, RPM, etc, along with Port Number

        void set_zero_position(int position) override;

        void shutDown() override;

        void revive() override;

        bool isOverheated() override;

        std::vector<bool> getIsBroken();

        std::vector<float> getAllRPM();

        float getAverageRPM();

        float getVoltage();

        float getAvgIMEPos();
};

} // namespace lemlib