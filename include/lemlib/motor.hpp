#pragma once
#include "main.h"
#include "pros/motors.h"
#include "lemlib/pid.hpp"
#include <memory>

class Abstract_Motor {
protected:

    static constexpr int overheatTempCelsius = 55;

    bool isBroken = false;
    bool isReversed = false;
    bool hasPairMotor = false;

    int voltage = 0;

    float gearRatio = 0;

    pros::motor_gearset_e gearset = pros::E_MOTOR_GEAR_GREEN;

    Abstract_Motor();

    virtual ~Abstract_Motor();

public:

    virtual void spinAtVoltage(int voltage) = 0;
 
    virtual void spinPerc(int percent) = 0;
 
    virtual void spinJoystick(int joystickValue) = 0;
 
    virtual void spinPercVEXPID(int percent) = 0;
 
    virtual void spinAtRPM(int RPM) = 0;
 
    virtual void spinUntilDegree(int degree, int speedInVoltage) = 0;
 
    virtual void spinFor(float seconds, int speedInVoltage) = 0;
 
    virtual void logMotorPerformance() = 0; // Stuff like wattage and temperature, RPM, etc, along with Port Number

    virtual void set_zero_position(int position) = 0;
 
    virtual void shutDown() = 0;
 
    virtual void revive() = 0;
    
    virtual bool isOverheated() = 0;
    
};

class PROSMotor : public Abstract_Motor {
private:

    std::unique_ptr<pros::Motor> motor;
    std::shared_ptr<PROSMotor> pairMotor;

    FAPID pid;

    static constexpr int overheatTempCelsius = 55;

    bool isBroken = false;
    bool isReversed = false;
    bool hasPairMotor = false;

    int voltage = 0;

    Gearing gearRatio;

    pros::motor_gearset_e gearset = pros::E_MOTOR_GEAR_GREEN;

public:

    /**
     * @brief Construct a new Uvuv Motor object
     * 
     * @param port V5 Smart Port number
     * @param isReversedArg Defines direction of the motor spinning. True is reversed, false is normal.
     * @param gearRatioArg Gear ratio of the motors mechanism. Accessed by "G_"
     * @param gearset The PROS motor gearset. 
     * @param pairMotorArg The motor paired with this motor. For example, leftFront and rightFront motors.
     * @param pid Built-in PID Controller for the motor.
     */
    UvuvMotor(uint8_t port, bool isReversedArg, Gearing gearRatioArg, pros::motor_gearset_e gearset = pros::E_MOTOR_GEAR_GREEN, 
        UvuvMotor* pairMotorArg = nullptr, PIDController* pidArg = nullptr);

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

class UvuvMotorGroup : public UvuvMotorParent {
private:
    
    std::vector<UvuvMotor*> motorContainer;

    PIDController* drivePID;

    PIDController* turnPID;

public:

    UvuvMotorGroup(std::vector<UvuvMotor*> motorContainerArg);

    UvuvMotorGroup(std::vector<UvuvMotor*>* motorContainerArg);

    UvuvMotorGroup(std::vector<std::pair<int, motorRotation>> motorParameters);

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

    std::vector<bool> getIsBroken();

    std::vector<float> getIndividualRPM();

    float getAverageRPM();

    float getVoltage();

    PIDController* getDrivePID();

    PIDController* getTurnPID();

    bool assignDrivePID(PIDController pid);

    bool assignTurnPID(PIDController pid);

    float getAverageEncoderPositions();

};