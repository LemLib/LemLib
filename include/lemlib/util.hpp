#pragma once


namespace lemlib {
    /**
    * @brief Slew rate limiter
    * 
    * @param target target value
    * @param current current value
    * @param maxChange maximum change. No maximum if set to 0
    * @return float - the limited value
    */
    float slew(float target, float current, float maxChange);
}
