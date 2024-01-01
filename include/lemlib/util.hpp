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
#include <math.h>
#include "lemlib/pose.hpp"

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

/**
 * @brief Convert radians to degrees
 *
 * @param rad radians
 * @return float degrees
 */
constexpr float radToDeg(float rad) { return rad * 180 / M_PI; }

/**
 * @brief Convert degrees to radians
 *
 * @param deg degrees
 * @return float radians
 */
constexpr float degToRad(float deg) { return deg * M_PI / 180; }

/**
 * @brief Calculate the error between 2 angles. Useful when calculating the error between 2 headings
 *
 * @param angle1
 * @param angle2
 * @param radians true if angle is in radians, false if not. true by default
 * @return float wrapped angle
 */
float angleError(float angle1, float angle2, bool radians = true);

/**
 * @brief Return the sign of a number
 *
 * @param x the number to get the sign of
 * @return int - -1 if negative, 1 if positive
 */
template <typename T> constexpr T sgn(T value) { return value < 0 ? -1 : 1; }

/**
 * @brief Return the average of a vector of numbers
 *
 * @param values
 * @return float
 */
float avg(std::vector<float> values);

/**
 * @brief Exponential moving average
 *
 * @param current current measurement
 * @param previous previous output
 * @param smooth smoothing factor (0-1). 1 means no smoothing, 0 means no change
 * @return float - the smoothed output
 */
float ema(float current, float previous, float smooth);

/**
 * @brief Get the signed curvature of a circle that intersects the first pose and the second pose
 *
 * @note The circle will be tangent to the theta value of the first pose
 * @note The curvature is signed. Positive curvature means the circle is going clockwise, negative means
 * counter-clockwise
 * @note Theta has to be in radians and in standard form. That means 0 is right and increases counter-clockwise
 *
 * @param pose the first pose
 * @param other the second pose
 * @return float curvature
 */
float getCurvature(Pose pose, Pose other);

/**
 * @brief function that returns elements in a string separated by a delimeter
 *
 * @param input the raw string
 * @param delimeter string separating the elements in the input string
 * @return std::vector<std::string> vector of elements read from the file
 */
std::vector<std::string> splitString(const std::string& input, const std::string& delimiter);

/**
 * @brief Finds the closest waypoint in a vector of waypoints to a target pose
 *
 * @param poses the vector of waypoints
 * @param target the target pose
 * @return Pose
 */
Waypoint closestWaypoint(const std::vector<Waypoint>& waypoints, const Pose& target);

/**
 * @brief Find the intersection between a circle and a line
 *
 *
 * @param p1 the start of the line
 * @param p2 the end of the line
 * @param center the center of the circle
 * @param radius the radius of the circle
 * @return Pose the intersection point. If no intersection is found, the circle center will be returned. If there are
 * multiple intersections, the first one will be returned
 */
Pose circleLineIntersect(Pose p1, Pose p2, Pose center, float radius);

/*
 * @brief Linearly interpolate two points
 *
 * @param x the x value to interpolate
 * @param x1 the x value of the lower adjacent coordinate
 * @param y1 the y value of the lower adjacent coordinate
 * @param x2 the x value of the upper adjacent coordinate
 * @param y2 the y value of the upper adjacent coordinate
 * @return the interpolated y value
 */
float linearInterp(float x, float x1, float y1, float x2, float y2);

} // namespace lemlib