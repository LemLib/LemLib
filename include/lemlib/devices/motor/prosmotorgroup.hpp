#pragma once

#include "abstractgroup.hpp"
#include "lemlib/devices/motor/abstractmotor.hpp"
#include "prosmotor.hpp"
#include <memory>

namespace lemlib {

class PROSMotorGroup : public Abstract_MotorGroup {
    private:
        
    public:
        PROSMotorGroup(std::vector<std::unique_ptr<PROSMotor>> motorContainerArg);

        PROSMotorGroup(std::vector<std::pair<MotorInfo, const int>> motorParameters);

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

        std::vector<double> getPositions() override;

        float getAverageRPM();

        float getVoltage();

        std::vector<std::unique_ptr<Abstract_Motor>>& getMotorContainer() override;
};

} // namespace lemlib