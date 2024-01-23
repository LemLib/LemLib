#include "lemlib/bangbang.hpp"

BangBangController::BangBangController(int thresholdValue, int BBVoltage, bool activWhenLower) {
    this->thresholdValue = thresholdValue;
    this->BBVoltage = BBVoltage;
    this->activWhenLower = activWhenLower;
}

int BangBangController::getBBVoltage(int currentValue) {
    if (activWhenLower) {
        if (currentValue < thresholdValue) {
            return BBVoltage;
        } else {
            return 0;
        }
    } else {
        if (currentValue > thresholdValue) {
            return BBVoltage;
        } else {
            return 0;
        }
    }

    return 0;
}