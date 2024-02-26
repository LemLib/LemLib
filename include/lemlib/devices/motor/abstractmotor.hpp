#pragma once
#include "main.h"
#include "pros/motors.h"
#include "lemlib/pid.hpp"
#include <memory>

namespace lemlib {

struct MotorInfo {
    int port;
    bool reversed;
    float gearRatio;

    MotorInfo(int port, bool reversed, float gearRatio) : port(port), reversed(reversed), gearRatio(gearRatio) {}
};

class Abstract_Motor {
protected:

    static constexpr int overheatTempCelsius = 55;

    bool isBroken = false;
    bool isReversed = false;
    bool hasPairMotor = false;

    int voltage = 0;

    float gearRatio = 0;

    float baseRPM = 0;

public:

    virtual void spinAtVoltage(int voltage) = 0;
 
    virtual void spinPerc(int percent) = 0;
 
    virtual void spinJoystick(int joystickValue) = 0;
 
    virtual void spinAtRPM(int RPM) = 0;
 
    virtual void spinUntilDegree(int degree, int speedInVoltage) = 0;
 
    virtual void spinFor(float seconds, int speedInVoltage) = 0;
 
    virtual void logMotorPerformance() = 0; // Stuff like wattage and temperature, RPM, etc, along with Port Number

    virtual void set_zero_position(int position) = 0;

    virtual int getPosition() = 0;
 
    virtual void shutDown() = 0;
 
    virtual void revive() = 0;
    
    virtual bool isOverheated() = 0;

    virtual bool getIsBroken();

    virtual float getRPM();

    virtual float getVoltage();

    virtual int getPort();

    virtual void setGearset(int gearset);

    virtual int getGearset();

    virtual void setReversed(bool isReversed);

    virtual bool getIsReversed();

    virtual std::shared_ptr<Abstract_Motor> getPairMotor();

    virtual void setPID(std::shared_ptr<FAPID> pid);

    virtual std::shared_ptr<FAPID> getPID();

    virtual void setGearRatio(float gearRatio);

    virtual float getGearRatio();
    
};


} // namespace lemlib