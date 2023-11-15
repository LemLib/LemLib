/**
 * @file src/lemlib/util.cpp
 * @author Liam Teale
 * @brief File containing definitions for utility functions
 * @version 0.4.5
 * @date 2023-01-15
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <math.h>
#include <vector>
#include <string.h>
#include "lemlib/pose.hpp"
#include "lemlib/util.hpp"

namespace lemlib {
/**
 * @brief Slew rate limiter
 *
 * @param target target value
 * @param current current value
 * @param maxChange maximum change. No maximum if set to 0
 * @return float - the limited value
 */
float slew(float target, float current, float maxChange) {
    float change = target - current;
    if (maxChange == 0) return target;
    if (change > maxChange) change = maxChange;
    else if (change < -maxChange) change = -maxChange;
    return current + change;
}

/**
 * @brief Calculate the error between 2 angles. Useful when calculating the error between 2 headings
 *
 * @param angle1
 * @param angle2
 * @param radians true if angle is in radians, false if not. False by default
 * @return float wrapped angle
 */
Angle angleError(Angle angle1, Angle angle2) {
    return angle2 - angle1; // todo test
}

/**
 * @brief Return the sign of a number
 *
 * @param x the number to get the sign of
 * @return int - -1 if negative, 1 if positive
 */
int sgn(float x) {
    if (x < 0) return -1;
    else return 1;
}

/**
 * @brief Exponential moving average
 *
 * @param current current measurement
 * @param previous previous output
 * @param smooth smoothing factor (0-1). 1 means no smoothing, 0 means no change
 * @return float - the smoothed output
 */
template <isQuantity Q> Q ema(Q current, Q previous, float smooth) {
    return (current * smooth) + (previous * (1 - smooth));
}

/**
 * Finds the curvature of a circle which intersects 2 points, and is tangent to the first point
 *
 * Inspired by: https://www.chiefdelphi.com/t/paper-implementation-of-the-adaptive-pure-pursuit-controller/166552
 */
Curvature getCurvature(Pose p1, Pose p2) {
    // calculate whether the pose is on the left or right side of the circle
    uint8_t side = units::sgn(units::sin(p1.theta) * (p2.x - p1.x) - units::cos(p1.theta) * (p2.y - p1.y));
    // calculate center point and radius
    Number a = -units::tan(p1.theta);
    Length c = units::tan(p1.theta) * p1.x - p1.y;
    Length x = units::abs(a * p2.x + p2.y + c) / units::sqrt((a * a) + Number(1));
    Length d = p1.distance(p2);

    // return curvature
    return side * rad * ((2 * x) / (d * d)); // todo needs testing
}

/**
 * Split a string into a vector of strings using a string delimeter
 *
 * This works by iterating through the string until there are no more instances of the delimeter.
 * For each instance of the delimeter, the part of the string before the delimeter is pushed to the vector.
 * The last part of the string is pushed to the vector after the loop is done.
 */
std::vector<std::string> splitString(const std::string& input, const std::string& delimiter) {
    std::vector<std::string> output;
    const size_t delimLength = delimiter.length();
    size_t start = 0;
    size_t end = input.find(delimiter);

    // loop through the string until there are no more instances of the delimeter
    while (end != std::string::npos) {
        output.push_back(input.substr(start, end - start));
        // skip the delimeter
        start = end + delimLength;
        end = input.find(delimiter, start);
    }

    // Push the last part of the string if there's any left
    output.push_back(input.substr(start));

    return output;
}

/**
 * Finds the closest waypoint in a vector of waypoints to a target point
 *
 * Calculates the distance between the target pose and each waypoint in the vector.
 * The waypoint with the smallest distance is returned.
 *
 * The function only calculates the square of the distance, because the exact distance is not needed.
 * It only needs to know what waypoint is closest.
 */
Waypoint closestWaypoint(const std::vector<Waypoint>& waypoints, const Pose& target) {
    Waypoint closest = waypoints[0];
    Area dist = units::square(target.x - closest.x) + units::square(target.y - closest.y);

    // loop through all path points
    for (const Waypoint& wp : waypoints) {
        const Area dist2 = units::square(target.x - wp.x) + units::square(target.y - wp.y);
        if (dist2 < dist) {
            closest = wp;
            dist = dist2;
        }
    }

    return closest;
}

/**
 * Find the intersection between a line segment and a circle given its center and radius
 *
 * Inspired by: https://stackoverflow.com/a/1084899
 *
 * It uses the quadratic formula to calculate the intersection points.
 * If there are no intersections, it returns the center of the circle.
 * If there are multiple intersections, it returns the first one.
 */
Pose circleLineIntersect(Pose p1, Pose p2, Pose center, Length radius) { // todo: test
    // uses the quadratic formula to calculate intersection points
    Pose d = p2 - p1;
    Pose f = p1 - center;
    Area a = units::square(d.x) + units::square(d.y);
    Area b = 2 * (f.x * d.x + f.y * d.y);
    Area c = units::square(f.x) + units::square(f.y) - units::square(radius);
    Quantity<std::ratio<0>, std::ratio<4>, std::ratio<0>, std::ratio<0>> discriminant = units::square(b) - (4 * a * c);

    // if a possible intersection was found
    if (discriminant.raw() >= 0) {
        Area discriminant1 = units::sqrt(discriminant);
        float t1 = ((-b - discriminant1) / (2 * a)).raw();
        float t2 = ((-b + discriminant1) / (2 * a)).raw();

        // prioritize first intersection
        float t = -1;
        if (t2 >= 0 && t2 <= 1) t = t2;
        if (t1 >= 0 && t1 <= 1) t = t1;
        if (t == -1) return center;

        // calculate x and y values of intersection point
        return p1.lerp(p2, t);
    }

    // no intersection found
    return center;
}
}; // namespace lemlib