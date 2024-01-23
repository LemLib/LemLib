#include "pros/motors.hpp"

class BangBangController {
    protected:
        int thresholdValue = 100;
        int BBVoltage = 127;
        bool activWhenLower = true;
    public:
        /**
         * @brief Construct a new Bang Bang Controller object
         *
         * @param thresholdRPM The value where Bang Bang activates
         * @param outputVoltage The Voltage that gets outputted when the system is under/over the threshold.
         * @param activWhenLower True if Bang Bang should activate when its lower than the value, False if higher.
         */
        BangBangController(int thresholdValue, int BBVoltage, bool activWhenLower = true);

        /**
         * @brief Get the voltage Bang Bang outputs. Its either the Voltage you put in the constructor or 0.
         *
         * @param currentValue The current value of the system you're controlling.
         * @return int
         */
        int getBBVoltage(int currentValue);
};