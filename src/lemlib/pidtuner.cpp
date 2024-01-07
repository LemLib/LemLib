#include "lemlib/pidtuner.hpp"
#include "pros/motor_group.hpp"
#include <vector>

namespace lemlib {

PIDTuner::PIDTuner(FAPID* pidArg, pros::MotorGroup motorGroup, bool tunekF, bool tunekA, bool tunekP, bool tunekI, bool tunekD) {
    
    gains = {(float)tunekF, (float)tunekA, (float)tunekP, (float)tunekI, (float)tunekD};

    this->pid = pidArg;

    this->pid->setGains(gains.kF, gains.kA, gains.kP, gains.kI, gains.kD);

}

PIDTuner::PIDTuner(FAPID* pidArg, std::vector<pros::MotorGroup> motorGroup, bool tunekF, bool tunekA, bool tunekP, bool tunekI, bool tunekD) {
    
    gains = {(float)tunekF, (float)tunekA, (float)tunekP, (float)tunekI, (float)tunekD};

    this->pid = pidArg;

    this->pid->setGains(gains.kF, gains.kA, gains.kP, gains.kI, gains.kD);

    for (int i = 0; i < motorGroup.size(); i++) {
        motorGroups.emplace_back(motorGroup.at(i));
    }

}

void PIDTuner::tuneFlywheel(int targetRPM, float gearRatio) {

    float avgVelocity = 0;

    std::vector<float> filteredVelocity;

    if (gains.kF == 0) {
        
        // While kF's value is not reaching around 100 RPM of the target RPM
        while (avgVelocity < targetRPM - 200 || avgVelocity > targetRPM) {
            
            avgVelocity = 0; // Reset the average velocity to 0

            for (int i = 0; i > 5; i++) {
                // Average out the velocity of all the motors
                for (int i = 0; i < motorGroups.size(); i++) {
                    avgVelocity += motorGroups.at(i).get_actual_velocity() * gearRatio;
                    avgVelocity /= motorGroups.size();
                    filteredVelocity.emplace_back(avgVelocity);
                }
            }

            // Get the Feed Forward output
            float motorVoltage = pid->update(targetRPM, avgVelocity);
            
            // Move the motors at the calculated voltage
            for (int i = 0; i < motorGroups.size(); i++) {
                motorGroups.at(i).move_voltage(motorVoltage);
            }

            // Depending on the current kF value, adjust it accordingly
            if (avgVelocity > targetRPM - 200) {
                gains.kF -= .1;
                pros::delay(1000);
            }
            else if (avgVelocity < targetRPM - 300) {
                gains.kF *= 2;
                pros::delay(3000);
            }
            else {
                gains.kF += .1;
                pros::delay(1000);
            }

            pros::delay(20);
        
        }
    }
    

    // kF is done!

    if (gains.kP == 0) {
        
        // While the average velocity is outside of 5 RPM of the target RPM
        while (avgVelocity > targetRPM + 5 || avgVelocity < targetRPM - 5) {
            
            avgVelocity = 0; // Reset the average velocity to 0

            // Average out the velocity of all the motors
            for (int i = 0; i < motorGroups.size(); i++) {
                avgVelocity += motorGroups.at(i).get_actual_velocity() * gearRatio;
                avgVelocity /= motorGroups.size();
            }

            // Get the Feed Forward output
            float motorVoltage = pid->update(targetRPM, avgVelocity);
            
            // Move the motors at the calculated voltage
            for (int i = 0; i < motorGroups.size(); i++) {
                motorGroups.at(i).move_voltage(motorVoltage);
            }

            // Depending on the current kF value, adjust it accordingly
            if (avgVelocity > targetRPM - 50) {
                gains.kF -= .1;
                pros::delay(1000);
            }
            else if (avgVelocity < targetRPM - 300) {
                gains.kF *= 2;
                pros::delay(3000);
            }
            else {
                gains.kF += .1;
                pros::delay(1000);
            }

            pros::delay(20);
        
        }
    }

    /*
    * While kP average velocity goes to within 5 RPM of the target value
    * 
    * 
    * 
    * 
    */

}

}