#include "lemlib/motions/follow.hpp"
#include "LemLog/logger/Helper.hpp"
#include "lemlib/MotionCancelHelper.hpp"
#include "lemlib/Timer.hpp"
#include "lemlib/util.hpp"

using namespace units;

namespace lemlib {

static logger::Helper logHelper("lemlib/motions/moveToPoint");

/**
 * @brief function that returns elements in a file line, separated by a delimeter
 *
 * @param input the raw string
 * @param delimeter string separating the elements in the line
 * @return std::vector<std::string> array of elements read from the file
 */
static std::vector<std::string> readElement(const std::string& input, const std::string& delimiter) {
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

    output.push_back(s); // add the last element to the returned string

    return output;
}

/**
 * @brief Convert a string to hex
 *
 * @param input the string to convert
 * @return std::string hexadecimal output
 */
static std::string stringToHex(const std::string& input) {
    static const char hex_digits[] = "0123456789ABCDEF";

    std::string output;
    output.reserve(input.length() * 2);
    for (unsigned char c : input) {
        output.push_back(hex_digits[c >> 4]);
        output.push_back(hex_digits[c & 15]);
    }
    return output;
}

class Waypoint : public V2Position {
    public:
        Waypoint(Length x, Length y, Number speed)
            : V2Position(x, y),
              speed(speed) {}

        Number speed;
};

/**
 * @brief Decode an asset and return the path
 *
 * @param asset The file to read from
 * @return std::vector<Pose> vector of points on the path
 */
static std::vector<Waypoint> getPath(const asset& asset) {
    std::vector<Waypoint> path;

    // format data from the asset
    const std::string data(reinterpret_cast<char*>(asset.buf), asset.size);
    const std::vector<std::string> dataLines = readElement(data, "\n");

    // read the points until 'endData' is read
    for (std::string line : dataLines) {
        logHelper.debug("read raw line {}", stringToHex(line));
        if (line == "endData" || line == "endData\r") break;
        const std::vector<std::string> pointInput = readElement(line, ", "); // parse line
        // check if the line was read correctly
        if (pointInput.size() != 3) {
            logHelper.error("Failed to read path file! Are you using the right format? Raw line: {}",
                            stringToHex(line));
            break;
        }
        Waypoint waypoint = {0_in, 0_in, 0};
        waypoint.x = from_in(std::stod(pointInput.at(0))); // x position
        waypoint.y = from_in(std::stod(pointInput.at(1))); // y position
        waypoint.speed = std::stod(pointInput.at(2)); // velocity
        path.push_back(waypoint); // save data
        logHelper.debug("read point {}", static_cast<const V2Position&>(waypoint));
    }

    return path;
}

/**
 * @brief find the closest point on the path to the robot
 *
 * @param pose the current pose of the robot
 * @param path the path to follow
 * @return int index to the closest point
 */
static int findClosest(V2Position pos, const std::vector<Waypoint>& path) {
    int closestPoint = 0;
    Length closestDist = pos.distanceTo(path.at(0));

    // loop through all path points
    for (int i = 1; i < path.size(); i++) {
        const Length dist = pos.distanceTo(path.at(i));
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
static Number findCircleIntersect(V2Position p1, V2Position p2, V2Position pos, Length lookaheadDist) {
    // calculations
    // uses the quadratic formula to calculate intersection points
    const V2Position d = p2 - p1;
    const V2Position f = p1 - pos;
    const auto a = d * d;
    const auto b = 2 * (f * d);
    const auto c = (f * f) - lookaheadDist * lookaheadDist;
    const auto discriminant = b * b - 4 * a * c;

    // if a possible intersection was found
    if (discriminant.internal() >= 0) {
        const auto temp = sqrt(discriminant);
        Number t1 = (-b - temp) / (2 * a);
        Number t2 = (-b + temp) / (2 * a);

        // prioritize further down the path
        if (t2 >= 0 && t2 <= 1) return t2;
        else if (t1 >= 0 && t1 <= 1) return t1;
    }

    // no intersection found
    return -1;
}

class LookaheadPoint : public V2Position {
    public:
        LookaheadPoint(Length x, Length y, int index)
            : V2Position(x, y),
              index(index) {}

        int index;
};

/**
 * @brief returns the lookahead point
 *
 * @param lastLookahead - the last lookahead point
 * @param pose - the current position of the robot
 * @param path - the path to follow
 * @param closest - the index of the point closest to the robot
 * @param lookaheadDist - the lookahead distance of the algorithm
 */
static LookaheadPoint findLookaheadPoint(LookaheadPoint lastLookaheadPoint, Pose pose,
                                         const std::vector<Waypoint>& path, int closest, Length lookaheadDist) {
    // optimizations applied:
    // only consider intersections that have an index greater than or equal to the point closest
    // to the robot
    // and intersections that have an index greater than or equal to the index of the last
    // lookahead point
    const int start = max(closest, lastLookaheadPoint.index);
    for (int i = start; i < path.size() - 1; i++) {
        const Waypoint lastWaypoint = path.at(i);
        const Waypoint waypoint = path.at(i + 1);

        const Number t = findCircleIntersect(lastWaypoint, waypoint, pose, lookaheadDist);

        if (t != -1) {
            const V2Position lookahead = lastWaypoint + (waypoint - lastWaypoint) * t;
            return {lookahead.x, lookahead.y, i};
        }
    }

    // robot deviated from path, use last lookahead point
    return lastLookaheadPoint;
}

void follow(const asset& asset, Length lookaheadDistance, Time timeout, FollowParams params, FollowSettings settings) {
    const std::vector<Waypoint> path = getPath(asset); // get list of path points
    if (path.size() == 0) {
        logHelper.error("No points in path! Do you have the right format? Skipping motion");
        return;
    }
    LookaheadPoint lastLookahead = {path.at(0).x, path.at(0).y, 0};
    Number prevVel = 0;

    lemlib::MotionCancelHelper helper(10_msec); // cancel helper
    Timer timer(timeout);
    while (!timer.isDone() && helper.wait()) {
        // get the current position of the robot
        const Pose pose = [&] {
            Pose out = settings.poseGetter();
            if (params.reversed) out.orientation -= 180_stDeg;
            return out;
        }();

        // find the closest point on the path to the robot
        int closestPoint = findClosest(pose, path);
        // if the robot is at the end of the path, then stop
        if (path.at(closestPoint).speed == 0) break;

        // find the lookahead point
        const LookaheadPoint lookaheadPose =
            findLookaheadPoint(lastLookahead, pose, path, closestPoint, lookaheadDistance);
        lastLookahead = lookaheadPose; // update last lookahead position

        // get the curvature of the arc between the robot and the lookahead point
        const Curvature curvature = getSignedTangentArcCurvature(pose, lookaheadPose);

        // get the target velocity of the robot
        const Number targetVel = [&] {
            Number out = path.at(closestPoint).speed;
            out = slew(out, prevVel, params.lateralSlew, helper.getDelta());
            prevVel = out;
            return out;
        }();
        // calculate target left and right velocities

        Number targetLeftVel = targetVel * (2 + curvature * settings.trackWidth) / 2;
        Number targetRightVel = targetVel * (2 - curvature * settings.trackWidth) / 2;

        // ratio the speeds to respect the max speed
        float ratio = max(abs(targetLeftVel), abs(targetRightVel)) / 127;
        if (ratio > 1) {
            targetLeftVel /= ratio;
            targetRightVel /= ratio;
        }

        // move the drivetrain
        if (params.reversed) {
            settings.leftMotors.move(-targetRightVel);
            settings.rightMotors.move(-targetLeftVel);
        } else {
            settings.leftMotors.move(targetLeftVel);
            settings.rightMotors.move(targetRightVel);
        }
    }

    // stop the robot
    settings.leftMotors.brake();
    settings.rightMotors.brake();
}
} // namespace lemlib