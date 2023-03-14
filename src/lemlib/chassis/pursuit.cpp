/**
 * @file lemlib/chassis/pursuit.cpp
 * @author Liam Teale
 * @brief Pure Pursuit implementation
 * @version 0.1
 * @date 2023-02-09
 * @copyright Copyright (c) 2023
 *
 */

// The implementation below is mostly based off of
// the document written by Dawgma
// Here is a link to the original document
// https://www.chiefdelphi.com/uploads/default/original/3X/b/e/be0e06de00e07db66f97686505c3f4dde2e332dc.pdf

#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include "pros/misc.hpp"
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/util.hpp"

/**
 * @brief function that returns elements in a file line, separated by a delimeter
 *
 * @param input the raw string
 * @param delimeter string separating the elements in the line
 * @return std::vector<std::string> array of elements read from the file
 */
std::vector<std::string> readElement(std::string input, std::string delimiter) {
    std::string token;
    std::string s = input;
    std::vector<std::string> output;
    size_t pos = 0;

    // main loop
    while ((pos = s.find(delimiter)) != std::string::npos) { // while there are still delimiters in the string
        token = s.substr(0, pos); // processed substring
        output.push_back(token);
        s.erase(0, pos + delimiter.length()); // remove the read substring
    }

    if (s.length() > 1) s.pop_back(); // delete the endline character at the end of the string
    output.push_back(s); // add the last element to the returned string

    return output;
}

/**
 * @brief Get a path from the sd card
 *
 * @param filePath The file to read from
 * @return std::vector<lemlib::Pose> vector of points on the path
 */
std::vector<lemlib::Pose> getData(std::string filePath) {
    std::vector<lemlib::Pose> robotPath;
    std::string line;
    std::vector<std::string> pointInput;
    std::ifstream file(filePath, std::ios::in);
    lemlib::Pose pathPoint(0, 0, 0);

    // read the points until 'endData' is read
    while (getline(file, line) && line != "endData") {
        pointInput = readElement(line, ", "); // parse line
        pathPoint.x = std::stof(pointInput.at(0)); // x position
        pathPoint.y = std::stof(pointInput.at(1)); // y position
        pathPoint.theta = std::stof(pointInput.at(2)); // velocity
        robotPath.push_back(pathPoint); // save data
    }

    file.close();
    return robotPath;
}

/**
 * @brief find the closest point on the path to the robot
 *
 * @param pose the current pose of the robot
 * @param path the path to follow
 * @return int index to the closest point
 */
int findClosest(lemlib::Pose pose, std::vector<lemlib::Pose> path) {
    int closestPoint;
    float closestDist = 1000000;
    float dist;

    // loop through all path points
    for (int i = 0; i < path.size(); i++) {
        dist = pose.distance(path.at(i));
        if (dist < closestDist) { // new closest point
            closestDist = dist;
            closestPoint = i;
        }
    }

    return closestPoint;
}

/**
 * @brief Function that finds the intersection point between a circle and a line
 *
 * @param p1 start point of the line
 * @param p2 end point of the line
 * @param pos position of the robot
 * @param path the path to follow
 * @return float how far along the line the
 */
float circleIntersect(lemlib::Pose p1, lemlib::Pose p2, lemlib::Pose pose, float lookaheadDist) {
    // calculations
    // uses the quadratic formula to calculate intersection points
    lemlib::Pose d = p2 - p1;
    lemlib::Pose f = p1 - pose;
    float a = d * d;
    float b = 2 * (f * d);
    float c = (f * f) - lookaheadDist * lookaheadDist;
    float discriminant = b * b - 4 * a * c;

    // if a possible intersection was found
    if (discriminant >= 0) {
        discriminant = sqrt(discriminant);
        float t1 = (-b - discriminant) / (2 * a);
        float t2 = (-b + discriminant) / (2 * a);

        // prioritize further down the path
        if (t2 >= 0 && t2 <= 1) return t2;
        else if (t1 >= 0 && t1 <= 1) return t1;
    }
    return -1; // no intersection found
}

/**
 * @brief returns the lookahead point
 *
 * @param lastLookaheadIndex - the index of the last lookahead point
 * @param lastLookahead - the last lookahead point
 * @param pos - the current position of the robot
 * @param path - the path to follow
 * @param forward - whether to go forward (true) or backwards (false)
 */
lemlib::Pose lookaheadPoint(lemlib::Pose lastLookahead, lemlib::Pose pose, std::vector<lemlib::Pose> path,
                            float lookaheadDist) {
    // initialize variables
    lemlib::Pose lookahead = lastLookahead;
    double t;

    // find the furthest lookahead point on the path
    for (int i = 0; i < path.size() - 1; i++) {
        t = circleIntersect(path.at(i), path.at(i + 1), pose, lookaheadDist);
        if (t != -1 && i >= lastLookahead.theta) { // new lookahead point found
            lookahead = path.at(i).lerp(path.at(i + 1), t);
            lookahead.theta = i;
        }
    }

    return lookahead;
}

/**
 * @brief Get the curvature of a circle that intersects the robot and the lookahead point
 *
 * @param pos the position of the robot
 * @param heading the heading of the robot
 * @param lookahead the lookahead point
 * @return double curvature
 */
double findLookaheadCurvature(lemlib::Pose pose, double heading, lemlib::Pose lookahead) {
    // calculate whether the robot is on the left or right side of the circle
    double side = lemlib::sgn(std::sin(heading) * (lookahead.x - pose.x) - std::cos(heading) * (lookahead.y - pose.y));
    // calculate center point and radius
    double a = -std::tan(heading);
    double c = std::tan(heading) * pose.x - pose.y;
    double x = std::fabs(a * lookahead.x + lookahead.y + c) / std::sqrt((a * a) + 1);
    double d = std::hypot(lookahead.x - pose.x, lookahead.y - pose.y);

    // return curvature
    return side * ((2 * x) / (d * d));
}

/**
 * @brief Move the chassis along a path
 *
 * @param filePath file path to the path. No need to preface it with /usd/
 * @param timeout the maximum time the robot can spend moving
 * @param lookahead the lookahead distance. Units in inches. Larger values will make the robot move faster but will
 * follow the path less accurately
 * @param reverse whether the robot should follow the path in reverse. false by default
 * @param maxSpeed the maximum speed the robot can move at
 * @param log whether the chassis should log the path on a log file. false by default.
 */
void lemlib::Chassis::follow(const char* filePath, int timeout, float lookahead, bool reverse, float maxSpeed,
                             bool log) {
    std::vector<lemlib::Pose> path = getData("/usd/" + std::string(filePath)); // get list of path points
    Pose pose(0, 0, 0);
    Pose lookaheadPose(0, 0, 0);
    Pose lastLookahead = path.at(0);
    lastLookahead.theta = 0;
    double curvature;
    float targetVel;
    float prevLeftVel = 0;
    float prevRightVel = 0;
    int closestPoint;
    float leftInput = 0;
    float rightInput = 0;
    int compState = pros::competition::get_status();

    // loop until the robot is within the end tolerance
    for (int i = 0; i < timeout / 10 && pros::competition::get_status() == compState; i++) {
        // get the current position of the robot
        pose = this->getPose(true);
        if (reverse) pose.theta -= M_PI;

        // find the closest point on the path to the robot
        closestPoint = findClosest(pose, path);
        // if the robot is at the end of the path, then stop
        if (path.at(closestPoint).theta == 0) break;

        // find the lookahead point
        lookaheadPose = lookaheadPoint(lastLookahead, pose, path, lookahead);
        lastLookahead = lookaheadPose; // update last lookahead position

        // get the curvature of the arc between the robot and the lookahead point
        double curvatureHeading = M_PI / 2 - pose.theta;
        curvature = findLookaheadCurvature(pose, curvatureHeading, lookaheadPose);

        // get the target velocity of the robot
        targetVel = path.at(closestPoint).theta;

        // calculate target left and right velocities
        float targetLeftVel = targetVel * (2 + curvature * drivetrain.trackWidth) / 2;
        float targetRightVel = targetVel * (2 - curvature * drivetrain.trackWidth) / 2;

        // ratio the speeds to respect the max speed
        float ratio = std::max(std::fabs(targetLeftVel), std::fabs(targetRightVel)) / maxSpeed;
        if (ratio > 1) {
            targetLeftVel /= ratio;
            targetRightVel /= ratio;
        }

        // update previous velocities
        prevLeftVel = targetLeftVel;
        prevRightVel = targetRightVel;

        // move the drivetrain
        if (reverse) {
            drivetrain.leftMotors->move(-targetRightVel);
            drivetrain.rightMotors->move(-targetLeftVel);
        } else {
            drivetrain.leftMotors->move(targetLeftVel);
            drivetrain.rightMotors->move(targetRightVel);
        }

        pros::delay(10);
    }

    // stop the robot
    drivetrain.leftMotors->move(0);
    drivetrain.rightMotors->move(0);
}
