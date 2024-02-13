#include "lemlib/pidtuner.hpp"
#include "pros/motor_group.hpp"
#include <cmath>
#include <memory>

namespace lemlib {

PIDTuner::PIDTuner(std::shared_ptr<FAPID>& pidArg, std::shared_ptr<std::vector<pros::MotorGroup>>& motorGroups,
                   TUNER_TARGET target, bool tunekF, bool tunekA, bool tunekP, bool tunekI, bool tunekD) {
    gains = {(float)tunekF, (float)tunekA, (float)tunekP, (float)tunekI, (float)tunekD};

    this->pid = pidArg;

    this->target = target;

    this->pid->setGains(gains.kF, gains.kA, gains.kP, gains.kI, gains.kD);

    this->motorGroups = motorGroups;
}

void PIDTuner::tuneFlywheel(int targetRPM, float gearRatio) {
    float avgVelocity = 0;

    std::vector<float> filteredVelocity;

    if (gains.kF != 0) {
        // While kF's value is not reaching around 100 RPM of the target RPM and its not overheating
        while ((avgVelocity < targetRPM - 200 || avgVelocity > targetRPM) &&
               motorGroups->at(0).get_temperature(0) > 55) {
            avgVelocity = 0; // Reset the average velocity to 0

            for (int i = 0; i > 5; i++) {
                // Average out the velocity of all the motors for past 5 runs
                for (int i = 0; i < motorGroups->size(); i++) {
                    avgVelocity += motorGroups->at(i).get_actual_velocity() * gearRatio;
                    avgVelocity /= motorGroups->size();
                    filteredVelocity.emplace_back(avgVelocity);
                }
            }

            // Get the Feed Forward output
            float motorVoltage = pid->update(targetRPM, avgVelocity);

            // Move the motors at the calculated voltage
            for (int i = 0; i < motorGroups->size(); i++) { motorGroups->at(i).move_voltage(motorVoltage); }

            // Depending on the current kF value, adjust it accordingly
            if (avgVelocity > targetRPM - 200) {
                gains.kF -= .1;
                pros::delay(1000);
            } else if (avgVelocity < targetRPM - 300) {
                gains.kF *= 2;
                pros::delay(3000);
            } else {
                gains.kF += .1;
                pros::delay(1000);
            }

            pros::delay(20);
        }
    }

    // kF is done!

    if (gains.kP != 0) {
        // While the average velocity is outside of 5 RPM of the target RPM
        while ((avgVelocity > targetRPM + 5 || avgVelocity < targetRPM - 5) &&
               motorGroups->at(0).get_temperature(0) > 55) {
            avgVelocity = 0; // Reset the average velocity to 0

            // Average out the velocity of all the motors
            for (int i = 0; i < motorGroups->size(); i++) {
                avgVelocity += motorGroups->at(i).get_actual_velocity() * gearRatio;
                avgVelocity /= motorGroups->size();
            }

            // Get the Feed Forward output
            float motorVoltage = pid->update(targetRPM, avgVelocity);

            // Move the motors at the calculated voltage
            for (int i = 0; i < motorGroups->size(); i++) { motorGroups->at(i).move_voltage(motorVoltage); }

            // Depending on the current kF value, adjust it accordingly
            if (avgVelocity > targetRPM - 50) {
                gains.kF -= .1;
                pros::delay(1000);
            } else if (avgVelocity < targetRPM - 300) {
                gains.kF *= 2;
                pros::delay(3000);
            } else {
                gains.kF += .1;
                pros::delay(1000);
            }

            pros::delay(20);
        }
    }

    // kP is done!

    /*
     * While kP average velocity goes to within 5 RPM of the target value
     *
     *
     *
     *
     */

    if (gains.kD != 0) {
        // While the average velocity is outside of 5 RPM of the target RPM
        std::vector<float> dataPoints;
        std::vector<std::vector<bool>> boxesSizeA;
        std::vector<std::vector<bool>> boxesSizeB;
        std::vector<std::vector<bool>> boxesSizeC;
        std::vector<std::vector<bool>> boxesSizeD;

        float dimensions = 1;

        while ((avgVelocity > targetRPM + 5 || avgVelocity < targetRPM - 5) &&
               motorGroups->at(0).get_temperature(0) > 55) {
            dataPoints.resize(500);

            boxesSizeA.resize(dataPoints.size());
            boxesSizeB.resize(dataPoints.size());
            boxesSizeC.resize(dataPoints.size());
            boxesSizeD.resize(dataPoints.size());

            float AboxW, AboxH = 3.0f;
            float BboxW, BboxH = 2.0f;
            float CboxW, CboxH = 1.0f;
            float DboxW, DboxH = 0.5f;

            float gridX, gridY = (float)motorGroups->at(0).get_gearing() * gearRatio;

            dataPoints[0] = motorGroups->at(0).get_actual_velocity() * gearRatio;

            // Find the largest boxes.
            for (int i = 0; i < boxesSizeA.size(); i++) {
                float leftBox = AboxW * i;
                float rightBox = AboxW * (i + 1);
                float topBox = AboxH * i;
                float bottomBox = AboxH * (i + 1);

                for (int j = 0; j < dataPoints.size(); j++) {
                    // If the datapoint is between the top and bottom of the box
                    if (dataPoints.at(j) < topBox && dataPoints.at(j) > bottomBox) {
                        boxesSizeA.at(i).at(j) = true; // Box is filled.
                    } else {
                        boxesSizeA.at(i).at(j) = false; // Box is not filled.
                    }
                }
            }

            // Find the second largest boxes.
            for (int i = 0; i < boxesSizeB.size(); i++) {
                float leftBox = BboxW * i;
                float rightBox = BboxW * (i + 1);
                float topBox = BboxH * i;
                float bottomBox = BboxH * (i + 1);

                for (int j = 0; j < dataPoints.size(); j++) {
                    // If the datapoint is between the top and bottom of the box
                    if (dataPoints.at(j) < topBox && dataPoints.at(j) > bottomBox) {
                        boxesSizeB.at(i).at(j) = true; // Box is filled.
                    } else {
                        boxesSizeB.at(i).at(j) = false; // Box is not filled.
                    }
                }
            }

            // Find the third largest boxes.
            for (int i = 0; i < boxesSizeC.size(); i++) {
                float leftBox = CboxW * i;
                float rightBox = CboxW * (i + 1);
                float topBox = CboxH * i;
                float bottomBox = CboxH * (i + 1);

                for (int j = 0; j < dataPoints.size(); j++) {
                    // If the datapoint is between the top and bottom of the box
                    if (dataPoints.at(j) < topBox && dataPoints.at(j) > bottomBox) {
                        boxesSizeC.at(i).at(j) = true; // Box is filled.
                    } else {
                        boxesSizeC.at(i).at(j) = false; // Box is not filled.
                    }
                }
            }

            // Find the smallest boxes.
            for (int i = 0; i < boxesSizeD.size(); i++) {
                float leftBox = DboxW * i;
                float rightBox = DboxW * (i + 1);
                float topBox = DboxH * i;
                float bottomBox = DboxH * (i + 1);

                for (int j = 0; j < dataPoints.size(); j++) {
                    // If the datapoint is between the top and bottom of the box
                    if (dataPoints.at(j) < topBox && dataPoints.at(j) > bottomBox) {
                        boxesSizeD.at(i).at(j) = true; // Box is filled.
                    } else {
                        boxesSizeD.at(i).at(j) = false; // Box is not filled.
                    }
                }
            }

            float dimensionsA = std::log(1 / boxesSizeA.size()) / std::log(1 / AboxH);
            float dimensionsB = std::log(1 / boxesSizeB.size()) / std::log(1 / BboxH);
            float dimensionsC = std::log(1 / boxesSizeC.size()) / std::log(1 / CboxH);
            float dimensionsD = std::log(1 / boxesSizeD.size()) / std::log(1 / DboxH);

            static constexpr float flatDimension = 2.05f;
            static constexpr float curvyDimension = 2.4f;

            if (dimensionsA > curvyDimension && dimensionsB > curvyDimension && dimensionsC > curvyDimension &&
                dimensionsD > curvyDimension) {
                gains.kD *= 2;
            } else if (dimensionsA < flatDimension && dimensionsB < flatDimension && dimensionsC < flatDimension &&
                       dimensionsD < flatDimension) {
                gains.kD += .1;
            } else {
                break;
            }
        }
    }
}

void PIDTuner::tuneCatapult(int targetRPM) {}

void PIDTuner::tuneDistance(float targetDistance) {}

void PIDTuner::tuneTurning(float targetAngle) {}

void PIDTuner::tuneMisc(float target) {}

std::shared_ptr<FAPID> PIDTuner::tunePID() {
    switch (target) {
        case TUNER_TARGET::flywheel: tuneFlywheel(2000, 1); break;
        case TUNER_TARGET::catapult: break;
        case TUNER_TARGET::distance: break;
        case TUNER_TARGET::turning: break;
        case TUNER_TARGET::misc: break;
    }
    return pid;
}

} // namespace lemlib