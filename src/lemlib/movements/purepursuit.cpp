#include <cmath>
#include "pros/misc.hpp"
#include "pros/rtos.hpp"
#include "lemlib/util.hpp"
#include "lemlib/movements/purepursuit.hpp"

namespace lemlib {
/**
 * Construct a new Pure Pursuit movement
 *
 * Most of the class members are initialized in an initializer list
 * But some members need to be configured in the body
 *
 * We also need to decode the path asset into a vector of waypoints.
 * We do this by converting the asset into a string, and then splitting this
 * string into lines. Then we iterate through each of these lines, and split
 * it into a vector of strings. We then convert each of these strings into
 * floats, and then push back a new waypoint to the path vector
 */
PurePursuit::PurePursuit(float trackWidth, const asset& path, float lookaheadDist, int timeout, bool forwards,
                         int maxSpeed)
    : Movement(),
      trackWidth(trackWidth),
      lookaheadDist(lookaheadDist),
      timeout(timeout),
      forwards(forwards),
      maxSpeed(maxSpeed) {
    // get the current competition state. If this changes, the movement will stop
    compState = pros::competition::get_status();
    // get the starting time
    startTime = pros::millis();
    // decode the asset
    // TODO: be able to pass in a buffer directly instead of covert to string
    std::string input(reinterpret_cast<char*>(path.buf), path.size);
    std::vector<std::string> lines = splitString(input, "\n");
    for (const std::string& line : lines) { // loop through all lines
        if (line == "endData" || line == "endData\r") break;
        std::vector<std::string> pointInput = splitString(line, ", "); // parse line
        const float x = std::stof(pointInput.at(0)); // x position
        const float y = std::stof(pointInput.at(1)); // y position
        const float speed = std::stof(pointInput.at(2)); // speed
        this->path.push_back({x, y, 0, speed}); // save data
    }
}

/**
 * Get the distance travelled during the movement
 *
 * This is useful if you want to wait until the robot has travelled a certain distance.
 * For example, you want the robot to engage a mechanism when it has travelled 10 inches.
 */
float PurePursuit::getDist() { return dist; }

/**
 * Pure Pursuit is a motion algorithm published by R. Craig Coulter in 1992
 * https://www.ri.cmu.edu/pub_files/pub3/coulter_r_craig_1992_1/coulter_r_craig_1992_1.pdf
 *
 * Pure pursuit is a path following algorithm. It works by finding the intersection between
 * an imaginary circle around the robot and the path. The robot then moves to the intersection
 * point by calculating the curvature of the arc formed by the robot and the intersection point.
 * This arc is also tangent to the heading of the robot.
 *
 * This algorithm does need a path to follow, and it can be generated using the LemLib
 * path generator (deprecated), or Path.JerryIO (https://path.jerryio.com). The path
 * determines the speed of the robot at each point, allowing for smooth deceleration
 * and a slower speed around corners, or a user defined speed.
 */
std::pair<int, int> PurePursuit::update(Pose pose) {
    // exit if the competition state has changed
    if (pros::competition::get_status() != compState) state = 1;
    // exit if the timeout has been reached
    if (pros::millis() - startTime > timeout) state = 1;
    // exit if the movement is done
    if (state == 1) return {128, 128};

    // add pi to theta if the robot is moving backwards
    if (!forwards) pose.theta = std::fmod(pose.theta + M_PI, 2 * M_PI);

    // update completion vars
    if (dist == 0) { // if dist is 0, this is the first time update() has been called
        dist = 0.0001;
        prevPose = pose;
    }
    dist += pose.distance(prevPose);
    prevPose = pose;

    // find the closest waypoint on the path
    Waypoint closest = closestWaypoint(path, pose);
    // if the robot is at the end of the path, then stop
    if (closest.speed == 0) {
        state = 1;
        return {128, 128};
    }

    // find the lookahead point
    Waypoint lookahead = prevLookahead; // default to previous lookahead in case lookahead is not found
    for (int i = path.size() - 1; i > prevLookahead.index; i--) {
        Pose last = path.at(i - 1);
        Pose cur = path.at(i);
        // calculate intersection between lookahead circle and line between two waypoints
        Pose intersect = circleLineIntersect(last, cur, pose, lookaheadDist);
        // if the intersection is not the robot's current position, then we have found the lookahead point
        if (intersect != pose) {
            lookahead = {intersect.x, intersect.y, 0, 0, i};
            break;
        }
    }
    prevLookahead = lookahead; // update previous lookahead position

    // get the curvature of the arc between the robot and the lookahead point
    float curvature = getCurvature(pose, lookahead);

    // get the target velocity of the robot
    float targetVel = closest.speed;
    // calculate target left and right velocities
    float leftVel = targetVel * (2 + curvature * trackWidth) / 2;
    float rightVel = targetVel * (2 - curvature * trackWidth) / 2;
    // ratio the speeds to respect the max speed
    float ratio = std::max(std::fabs(leftVel), std::fabs(rightVel)) / maxSpeed;
    if (ratio > 1) {
        leftVel /= ratio;
        rightVel /= ratio;
    }
    // swap and reverse velocities if the robot is moving backwards
    if (!forwards) {
        std::swap(leftVel, rightVel);
        leftVel *= -1;
        rightVel *= -1;
    }

    return {std::round(leftVel), std::round(rightVel)};
}
}; // namespace lemlib