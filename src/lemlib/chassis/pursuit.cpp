/**
 * @file lemlib/chassis/pursuit.cpp
 * @author Liam Teale
 * @brief 
 * @version 0.1
 * @date 2023-02-09
 * Definition of the chassis .follow() function
 * @copyright Copyright (c) 2023
 * 
 */

#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include "lemlib/chassis/chassis.hpp"
#include "lemlib/util.hpp"











































/**
 * @brief function that returns elements in a file line, separated by a delimeter
 *
 * @param input the raw string
 * @param delimeter string separating the elements in the line
 * @return std::vector<std::string> array of elements read from the file
 */
std::vector<std::string> read_element(std::string input, std::string delimiter) 
{
    std::string token;
    std::string s = input;
    std::vector<std::string> output;

    size_t pos = 0;
    // main loop
    while (s != "endData") { // while there are still delimiters in the string
        token = s.substr(0, pos); // processed substring
        output.push_back(token);
        s.erase(0, pos + delimiter.length()); // remove the read substring
    }

    if (s.length() > 1) {
        s.pop_back(); // delete the endline character at the end of the string
    }
    output.push_back(s); // add the last element to the returned string

    return output;
}


/**
 * @brief Get a path from the sd card
 * @param filePath The file to read from
 *
 * @return path
 */
std::vector<lemlib::Pose> getData(std::string filePath, float targetHeading)
{
    // init variables
    std::vector<lemlib::Pose> robotPath;
    std::string line;
    std::vector<std::string> pointInput;
    std::ifstream file(filePath);
    lemlib::Pose pathPoint(0, 0, 0);

    // read the points until 'endData' is read
    while (getline(file, line) && line != "endData") {
        pointInput = read_element(line, ", ");
        pathPoint.x = std::stof(pointInput.at(0)); // x position
        pathPoint.y = std::stof(pointInput.at(1)); // y position
        pathPoint.theta = std::stof(pointInput.at(2)); // velocity
        robotPath.push_back(pathPoint);
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
int findClosest(lemlib::Pose pose, std::vector<lemlib::Pose> path)
{
    int closestPoint;
    float closestDist = 1000000;
    float dist;
    for (int i = 0; i < path.size()-1; i++) {
        dist = pose.distance(path.at(i));
        if (dist < closestDist) {
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
float circleIntersect(lemlib::Pose p1, lemlib::Pose p2, lemlib::Pose pose, std::vector<lemlib::Pose> path, float lookaheadDist) 
{
    lemlib::Pose d = p2 - p1;
    lemlib::Pose f = p1 - pose;

    float a = d * d;
    float b = 2 * (f * d);
    float c = (f* f) - lookaheadDist*lookaheadDist;
    float discriminant = b*b - 4*a*c;
    
    if (discriminant >= 0) {
        discriminant = sqrt(discriminant);
        float t1 = (-b-discriminant) / (2*a);
        float t2 = (-b+discriminant) / (2*a);

        /* prioritize further down path */
        if (t2 >= 0 && t2 <= 1) {
            return t2;
        } else if (t1 >= 0 && t1 <= 1) {
            return t1;
        } else { /**< no intersection was found */
            return -1;
        }

    } else { /**< no intersection was found */
        return -1;
    }
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
lemlib::Pose lookaheadPoint(int lastLookaheadIndex, lemlib::Pose lastLookahead, lemlib::Pose pose, std::vector<lemlib::Pose> path, float lookaheadDist)
{
    // initialize variables
    lemlib::Pose lookahead = lastLookahead;
    double t;

    // find the furthest lookahead point on the path
    for (int i = 0; i < path.size()-2; i++) {
        t = circleIntersect(path.at(i), path.at(i+1), pose, path, lookaheadDist);
        if (t != -1 && i >= lastLookaheadIndex) {
            lookahead = path.at(i).lerp(path.at(i+1), t);
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
double findLookaheadCurvature(lemlib::Pose pose, double heading, lemlib::Pose lookahead) 
{ 
    double side = lemlib::sgn(std::sin(heading)*(lookahead.x - pose.x) - std::cos(heading)*(lookahead.y - pose.y));
    double a = -std::tan(heading);
    double c = std::tan(heading)*pose.x - pose.y;
    double x = std::fabs(a * lookahead.x + lookahead.y + c) / std::sqrt((a*a) + 1);
    double d = std::hypot(lookahead.x - pose.x, lookahead.y - pose.y);
    return side * ((2*x) / (d*d));
}


/**
 * @brief calculate how to move the motors on the drivetrain
 * 
 * @param path the path to follow
 * @param leftInput the left motor velocity
 * @param rightInput the right motor velocity
 * @param curvature the curvature of the arc between the robot and the lookahead point
 * @param targetVel the target velocity of the robot
 * @param forward whether the robot should go forward (true) or backwards (false)
 * @param leftPID the left drivetrain PID
 * @param rightPID the right drivetrain PID
 */
void moveMotors(std::vector<lemlib::Pose> path, float leftInput, float rightInput, float curvature, float targetVel, bool forward, PID* leftPID, PID* rightPID)
{
    float targetLeftVel = targetVel * (2 + curvature*TRACK_WIDTH) / 2;
    float targetRightVel = targetVel * (2 - curvature*TRACK_WIDTH) / 2;

    float leftOutput = leftPID->calc(leftInput, targetLeftVel);
    float rightOutput = rightPID->calc(rightInput, targetRightVel);
    
    if (forward) {
        leftDrive.move(leftOutput);
        rightDrive.move(rightOutput);
    } else {
        leftDrive.move(-rightOutput);
        rightDrive.move(-leftOutput);
    }
}



/**
 * @brief follow the path
 * 
 * @param path the path to follow
 * @param forward whether the robot should go forwards (true) or backwards (false)
 * @param name the name of the debug file
 * @param smallErrorThreshold the threshold for small error
 * @param smallErrorTimeout the timeout for small error
 * @param largeErrorThreshold the threshold for large error
 * @param largeErrorTimeout the timeout for large error
 * @param maxTime the maximum time to follow the path
 */
void pursuit(Path* path, bool forward, std::string name, float smallErrorThreshold, float smallErrorTimeout, float largeErrorThreshold, float largeErrorTimeout, float maxTime)
{
    // log path
    logPath(path, name);
    // initialize timeouts
    float smallErrorTimer = -1;
    float largeErrorTimer = -1;
    float startTime = pros::millis();
    // initialize variables
    Point pos;
    Point lookahead;
    Point lastLookahead = path->points.at(0);
    lastLookahead.velocity = 0;
    double heading;
    double curvature;
    float targetVel;
    int closestPoint;
    bool finishedPursuit = false;
    bool finishedPID = false;
    float leftInput = 0;
    float rightInput = 0;
    // drivetrain PIDS
    PID leftPID("left");
    PID rightPID("right");
    leftPID.setParams(2, 0, 0, 0, 0, 0, 1, 1);
    rightPID.setParams(2, 0, 0, 0, 0, 0, 1, 1);

    PID lateralPID("lateral");
    PID angularPID("turning");
    lateralPID.setParams(0, 0, LATERAL_J, LATERAL_P, 0, LATERAL_D, 1, 1);
    lateralPID.setExit(0.5, 100, 2, 2000, 20000);
    angularPID.setParams(0, 0, ANGULAR_J, ANGULAR_P, 0, ANGULAR_D, 1, 1);
    leftPID.setLog(false);
    rightPID.setLog(false);

    // open debug file
    std::ofstream debugFile;
    debugFile.open("/usd/" + name + ".txt", std::ios_base::app);

    // loop until the robot is within the end tolerance
    for (int i = 0; i < maxTime/10; i++) {
        // get the current position of the robot
        pos.x = getX();
        pos.y = getY();

        // decide whether the robot should do the path forwards or backwards
        if (forward) {
            heading = getOrientation(true);
        } else {
            heading = getOrientation(true) - M_PI;
        }

        // check if the robot is within the end tolerance
        if (dist(pos, path->points.at(path->points.size()-1)) < path->pidActiveDist) {
            finishedPursuit = true;
        }
        // find the closest point on the path to the robot
        closestPoint = findClosest(pos, path);

        // find the lookahead point
        lookahead = lookaheadPoint(lastLookahead.velocity, lastLookahead, pos, path);
        lastLookahead = lookahead;

        // get the curvature of the arc between the robot and the lookahead point
        double curvatureHeading = M_PI/2 - heading;
        curvature = findLookaheadCurvature(pos, curvatureHeading, lookahead);

        // get the target velocity of the robot
        targetVel = path->points.at(closestPoint).velocity;

        // move the motors
        if (finishedPursuit) {
            break;
        } else {
            leftInput = rpmToSpeed(leftFront.get_actual_velocity(), 3.25, GEAR_RATIO);
            rightInput = rpmToSpeed(rightFront.get_actual_velocity(), 3.25, GEAR_RATIO);
            moveMotors(path, leftInput, rightInput, curvature, targetVel, forward, &leftPID, &rightPID);
        }

        pros::delay(10);
    }
    debugFile.close();

    // stop the robot
    leftDrive.move_voltage(0);
    rightDrive.move_voltage(0);
}





















































































/**
 * @brief Move the chassis along a path
 * 
 * @param path file path to the path. No need to preface it with /usd/
 * @param timeout the maximum time the robot can spend moving
 * @param lookahead the lookahead distance. Units in inches. Larger values will make the robot move faster but will follow the path less accurately
 * @param maxSpeed the maximum speed the robot can move at
 * @param log whether the chassis should log the path on a log file. false by default.
 */
void lemlib::Chassis::follow(const char *path, int timeout, float lookahead, float maxSpeed, bool log)
{

}
