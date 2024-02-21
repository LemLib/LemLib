#include "lemlib/devices/motor/prosmotor.hpp"
#include "pros/abstract_motor.hpp"
#include <memory>

namespace lemlib {

PROSMotor::PROSMotor(const uint8_t port, const bool isReversed, float gearRatioArg, const pros::v5::MotorGears gearset, 
    std::shared_ptr<PROSMotor> pairMotor, std::shared_ptr<FAPID> pid) {
        this->motor = std::make_unique<pros::Motor>(port, gearset);
        this->motor->set_reversed(isReversed);
        this->isReversed = isReversed;
        this->gearRatio = gearRatioArg;
        this->pairMotor = pairMotor;
        this->pid = pid;
        this->isBroken = false;

}

bool PROSMotor::getIsBroken() {
    return isBroken;
}

float PROSMotor::getRPM() {
    return motor->get_actual_velocity() * gearRatio;
}

float PROSMotor::getVoltage() {
    return voltage;
}

float PROSMotor::getEncoderPos() {
    return motor->get_position();
}

void PROSMotor::spinAtVoltage(int voltage) {
    if (!isBroken) {
        motor->move_voltage(voltage);
    }
    
}

void PROSMotor::spinPerc(int percent) {
    if (!isBroken) {
        motor->move_voltage(percent / 100 * 12000);
    }
}

void PROSMotor::spinJoystick(int joystickValue) {
    if (!isBroken) {
        motor->move_voltage(joystickValue / 127 * 12000);
    }
}

void PROSMotor::spinPercVEXPID(int percent) {
    if (!isBroken) {
        motor->move(voltage);
    }
}

void PROSMotor::spinAtRPM(int RPM) {
    if (!isBroken) {
        pid->update(getRPM(), RPM);
    }
}

void PROSMotor::spinUntilDegree(int degree, int speedInVoltage) {
    if (getEncoderPos() < degree) {
        spinAtVoltage(speedInVoltage);
    }
    else {
        spinAtVoltage(0);
    }
}

void PROSMotor::spinFor(float seconds, int speedInVoltage) {
    if (!isBroken) {
        static float startTime = pros::millis();

        if (pros::millis() - startTime < seconds * 1000) {
            spinAtVoltage(speedInVoltage);
        }
        else {
            spinAtVoltage(0);
        }
    }
    

}

void PROSMotor::logMotorPerformance() {

}

void PROSMotor::set_zero_position(int position) {
    motor->set_zero_position(position);
}

void PROSMotor::shutDown() {
    isBroken = true;
}

void PROSMotor::revive() {
    isBroken = false;
}

bool PROSMotor::isOverheated() {
    if (motor->get_temperature() > overheatTempCelsius) {
        return true;
    }
    else {
        return false;
    }
}

} // namespace lemlib
