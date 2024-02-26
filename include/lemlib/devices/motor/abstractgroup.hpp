#pragma once
#include "lemlib/devices/motor/abstractmotor.hpp"

namespace lemlib {

class Abstract_MotorGroup {
    protected:
        bool isBroken = false;

        std::vector<std::unique_ptr<Abstract_Motor>> motorContainer;
    public:
        virtual void spinAtVoltage(int voltage) = 0;

        virtual void spinPerc(int percent) = 0;

        virtual void spinJoystick(int joystickValue) = 0;

        virtual void spinAtRPM(int RPM) = 0;

        virtual void spinUntilDegree(int degree, int speedInVoltage) = 0;

        virtual void spinFor(float seconds, int speedInVoltage) = 0;

        virtual void logMotorPerformance() = 0; // Stuff like wattage and temperature, RPM, etc, along with Port Number

        virtual void shutDown() = 0;

        virtual void revive() = 0;

        virtual bool isOverheated() = 0;

        virtual void set_zero_position(int position) = 0;

        virtual std::vector<double> getPositions() = 0;

        virtual std::vector<int> getBaseRPMs() = 0;

        virtual std::vector<std::unique_ptr<Abstract_Motor>>& getMotorContainer() = 0;
};

} // namespace lemlib