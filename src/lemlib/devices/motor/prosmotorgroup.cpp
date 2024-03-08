#include "lemlib/devices/motor/prosmotorgroup.hpp"
#include "lemlib/devices/motor/abstractmotor.hpp"
#include <memory>

namespace lemlib {

PROSMotorGroup::PROSMotorGroup(std::vector<std::unique_ptr<PROSMotor>> motorContainerArg) {
    std::cout << motorContainerArg.size() << std::endl;
    for (int i = 0; i < motorContainerArg.size(); i++) {
        int port = motorContainerArg.at(i)->getPort();
        bool isReversed = motorContainerArg.at(i)->getIsReversed();
        float gearRatio = motorContainerArg.at(i)->getGearRatio();
        int gearset = motorContainerArg.at(i)->getGearset();

        motorContainer.emplace_back(std::make_unique<PROSMotor>(port, isReversed, gearRatio, gearset));
    }
}

PROSMotorGroup::PROSMotorGroup(std::vector<MotorInfo> motorParameters) {
    for (int i = 0; i < motorParameters.size(); i++) {
        int port = motorParameters.at(i).port;
        float gearRatio = motorParameters.at(i).gearRatio;
        bool isReversed = motorParameters.at(i).reversed;
        int gearset = motorParameters.at(i).cartrpm;

        motorContainer.emplace_back(std::make_unique<PROSMotor>(port, isReversed, gearRatio, gearset));
    }
}

void PROSMotorGroup::spinAtVoltage(int voltage) {
    static bool isAnyBroken = false;

    for (int i = 0; i < motorContainer.size(); i++) {
        if (motorContainer.at(i)->getIsBroken()) { isAnyBroken = true; }
    }

    if (!isAnyBroken) {
        for (int i = 0; i < motorContainer.size(); i++) { motorContainer.at(i)->spinAtVoltage(voltage); }
    }
}

void PROSMotorGroup::spinPerc(int percent) {
    bool isAnyBroken = false;
    std::cout << "Attempting to spin...     " << motorContainer.size();

    for (int i = 0; i < motorContainer.size(); i++) {
        if (motorContainer.at(i)->getIsBroken()) {
            std::cout << "BROKE";
            isAnyBroken = true;
        }
    }

    if (!isAnyBroken) {
        for (int i = 0; i < motorContainer.size(); i++) {
            std::cout << "SPUN AT PERCENT" << std::endl;
            motorContainer.at(i)->spinPerc(percent);
        }
    }
}

void PROSMotorGroup::spinJoystick(int joystickValue) {
    static bool isAnyBroken = false;

    for (int i = 0; i < motorContainer.size(); i++) {
        if (motorContainer.at(i)->getIsBroken()) { isAnyBroken = true; }
    }

    if (!isAnyBroken) {
        for (int i = 0; i < motorContainer.size(); i++) { motorContainer.at(i)->spinJoystick(joystickValue); }
    }
}

void PROSMotorGroup::spinAtRPM(int RPM) {
    static bool isAnyBroken = false;

    for (int i = 0; i < motorContainer.size(); i++) {
        if (motorContainer.at(i)->getIsBroken()) { isAnyBroken = true; }
    }

    if (!isAnyBroken) {
        for (int i = 0; i < motorContainer.size(); i++) { motorContainer.at(i)->spinAtRPM(RPM); }
    }
}

void PROSMotorGroup::spinUntilDegree(int degree, int speedInVoltage) {
    static bool isAnyBroken = false;

    for (int i = 0; i < motorContainer.size(); i++) {
        if (motorContainer.at(i)->getIsBroken()) { isAnyBroken = true; }
    }

    if (!isAnyBroken) {
        for (int i = 0; i < motorContainer.size(); i++) {
            motorContainer.at(i)->spinUntilDegree(degree, speedInVoltage);
        }
    }
}

void PROSMotorGroup::spinFor(float seconds, int speedInVoltage) {
    static bool isAnyBroken = false;

    for (int i = 0; i < motorContainer.size(); i++) {
        if (motorContainer.at(i)->getIsBroken()) { isAnyBroken = true; }
    }

    if (!isAnyBroken) {
        for (int i = 0; i < motorContainer.size(); i++) { motorContainer.at(i)->spinFor(seconds, speedInVoltage); }
    }
}

void PROSMotorGroup::logMotorPerformance() {}

void PROSMotorGroup::set_zero_position(int position) {
    for (int i = 0; i < motorContainer.size(); i++) { motorContainer.at(i)->set_zero_position(position); }
}

void PROSMotorGroup::shutDown() {
    for (int i = 0; i < motorContainer.size(); i++) { motorContainer.at(i)->shutDown(); }
}

void PROSMotorGroup::revive() {
    for (int i = 0; i < motorContainer.size(); i++) { motorContainer.at(i)->revive(); }
}

bool PROSMotorGroup::isOverheated() {
    for (int i = 0; i < motorContainer.size(); i++) {
        if (motorContainer.at(i)->isOverheated()) { return true; }
    }

    return false;
}

std::vector<bool> PROSMotorGroup::getIsBroken() {
    std::vector<bool> isBrokenContainer;

    for (int i = 0; i < motorContainer.size(); i++) {
        isBrokenContainer.push_back(motorContainer.at(i)->getIsBroken());
    }

    return isBrokenContainer;
}

std::vector<float> PROSMotorGroup::getAllRPM() {
    std::vector<float> RPMContainer;

    for (int i = 0; i < motorContainer.size(); i++) { RPMContainer.push_back(motorContainer.at(i)->getRPM()); }

    return RPMContainer;
}

float PROSMotorGroup::getAverageRPM() {
    float averageRPM = 0;

    for (int i = 0; i < motorContainer.size(); i++) { averageRPM += motorContainer.at(i)->getRPM(); }

    averageRPM /= motorContainer.size();

    return averageRPM;
}

float PROSMotorGroup::getVoltage() {
    float voltage = 0;

    for (int i = 0; i < motorContainer.size(); i++) { voltage += motorContainer.at(i)->getVoltage(); }

    voltage /= motorContainer.size();

    return voltage;
}

std::vector<double> PROSMotorGroup::getPositions() {
    
    std::vector<double> positions;
    for (int i = 0; i < motorContainer.size(); i++) {
        positions.push_back(motorContainer.at(i)->getPosition());
    }
    return positions;

}

std::vector<std::unique_ptr<Abstract_Motor>>& PROSMotorGroup::getMotorContainer() {
    
    return motorContainer;
}

std::vector<int> PROSMotorGroup::getBaseRPMs() {
    std::vector<int> baseRPMs;

    for (int i = 0; i < motorContainer.size(); i++) {
        baseRPMs.push_back(motorContainer.at(i)->getGearset());
    }

    return baseRPMs;
}

} // namespace lemlib