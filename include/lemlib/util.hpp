/**
 * @file include/lemlib/util.hpp
 * @author LemLib Team
 * @brief Utility functions declarations
 * @version 0.4.5
 * @date 2023-01-15
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <stdexcept>
#include <cmath>

namespace lemlib {
namespace util {
/**
 * @brief Slew rate limiter
 *
 * @param target target value
 * @param current current value
 * @param maxChange maximum change. No maximum if set to 0

 * @return float - the limited value
 */
float slew(float target, float current, float maxChange);

/**
 * @brief Convert radians to degrees
 *
 * @param rad radians
 * @return float degrees
 */
float radToDeg(float rad);

/**
 * @brief Convert degrees to radians
 *
 * @param deg degrees
 * @return float radians
 */
float degToRad(float deg);

/**
 * @brief Calculate the error between 2 angles. Useful when calculating the error between 2 headings
 *
 * @param angle1
 * @param angle2
 * @param radians true if angle is in radians, false if not. False by default
 * @return float wrapped angle
 */
float angleError(float angle1, float angle2, bool radians = false);

/**
 * @brief Return the sign of a number
 *
 * @param x the number to get the sign of
 * @return float - -1 if negative, 1 if positive
 */
float sgn(float x);

/**
 * @brief Return the average of a vector of numbers
 *
 * @tparam T the type of the vector
 * @param values the vector of numbers
 * @return T the average
 */
template <typename T>
T avg(std::vector<T> values) {
    T sum = 0;
    int size = 0;
    for (T value : values) {
        if (!std::isnan(value) && !std::isinf(value)) {
            sum += value;
            size++;
        }
    }
    if (size == 0) return 0;
    return sum / size;
}

/**
 * @brief Format a string with arguments
 *
 * @param format the format string
 * @return std::string the formatted string
 */
template <typename... Args>
std::string format(const std::string& format, Args... args) {
    int size_s = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
    if (size_s <= 0) { throw std::runtime_error("Error during formatting."); }
    auto size = static_cast<size_t>(size_s);
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}
} // namespace util
} // namespace lemlib
