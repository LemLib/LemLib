#pragma once

#include "pros/rtos.hpp"
#include "pros/imu.hpp"
#include "lemlib/asset.hpp"
#include "lemlib/chassis/trackingWheel.hpp"
#include "lemlib/pose.hpp"
#include "lemlib/pid.hpp"
#include "lemlib/exitcondition.hpp"
#include "lemlib/driveCurve.hpp"

namespace lemlib {

/**
 * @brief class containing the sensors used for odometry
 */
class OdomSensors {
    public:
        /**
         * The sensors are stored in a class so that they can be easily passed to the chassis class
         * The variables are pointers so that they can be set to nullptr if they are not used
         * Otherwise the chassis class would have to have a constructor for each possible combination of sensors
         *
         * @param vertical1 pointer to the first vertical tracking wheel
         * @param vertical2 pointer to the second vertical tracking wheel
         * @param horizontal1 pointer to the first horizontal tracking wheel
         * @param horizontal2 pointer to the second horizontal tracking wheel
         * @param imu pointer to the IMU
         *
         * @b Example
         * @code {.cpp}
         * pros::Rotation vertical_rotation(1); // rotation sensor on port 1
         * pros::Imu imu(2); // IMU on port 2
         * // tracking wheel using a new 2.75" wheel, 0.5 inches to the right of the tracking center
         * lemlib::TrackingWheel vertical1(&vertical_rotation, lemlib::Omniwheel::NEW_275, 0.5);
         * lemlib::OdomSensors sensors(&vertical1, // vertical tracking wheel
         *                     nullptr, // no second vertical tracking wheel, set to nullptr
         *                     nullptr, // no horizontal tracking wheels, set to nullptr
         *                     nullptr, // no second horizontal tracking wheel, set to nullptr
         *                     &imu); // IMU
         * @endcode
         */
        OdomSensors(TrackingWheel* vertical1, TrackingWheel* vertical2, TrackingWheel* horizontal1,
                    TrackingWheel* horizontal2, pros::Imu* imu);
        TrackingWheel* vertical1;
        TrackingWheel* vertical2;
        TrackingWheel* horizontal1;
        TrackingWheel* horizontal2;
        pros::Imu* imu;
};

/**
 * @brief class containing constants for a chassis controller
 */
class ControllerSettings {
    public:
        /**
         * @brief ControllerSettings constructor
         *
         * The constants are stored in a class so that they can be easily passed to the chassis class
         * Set a constant to 0 and it will be ignored
         *
         * @param kP proportional gain
         * @param kI integral gain
         * @param kD derivative gain
         * @param antiWindup integral anti windup range. If error is within this range, integral is set to 0
         * @param smallError range of error at which the chassis controller will exit if the error is within this range
         * for an amount of time determined by smallErrorTimeout
         * @param smallErrorTimeout the time the chassis controller will wait before exiting if error is within a
         * certain range determined by smallError
         * @param largeError range of error at which the chassis controller will exit if the error is within this range
         * for an amount of time determined by largeErrorTimeout
         * @param largeErrorTimeout the time the chassis controller will wait before exiting if error is within a
         * certain range determined by largeError
         * @param slew maximum acceleration
         *
         * @b Example
         * @code {.cpp}
         * lemlib::ControllerSettings lateralSettings(10, // proportional gain (kP)
         *                                            0, // integral gain (kI), set to 0 to disable
         *                                            3, // derivative gain (kD), set to 3
         *                                            3, // integral anti windup range, set to 0 to disable
         *                                            1, // small error range, in inches
         *                                            100, // small error range timeout, in milliseconds
         *                                            3, // large error range, in inches
         *                                            500, // large error range timeout, in milliseconds
         *                                            5); // maximum acceleration (slew)
         * @endcode
         */
        ControllerSettings(float kP, float kI, float kD, float windupRange, float smallError, float smallErrorTimeout,
                           float largeError, float largeErrorTimeout, float slew)
            : kP(kP),
              kI(kI),
              kD(kD),
              windupRange(windupRange),
              smallError(smallError),
              smallErrorTimeout(smallErrorTimeout),
              largeError(largeError),
              largeErrorTimeout(largeErrorTimeout),
              slew(slew) {}

        float kP;
        float kI;
        float kD;
        float windupRange;
        float smallError;
        float smallErrorTimeout;
        float largeError;
        float largeErrorTimeout;
        float slew;
};

/**
 * @brief class containing constants for a drivetrain
 */
class Drivetrain {
    public:
        /**
         * @brief Drivetrain constructor
         *
         * The constants are stored in a class so that they can be easily passed to the chassis class
         * Set a constant to 0 and it will be ignored
         *
         * @param leftMotors pointer to the left motors
         * @param rightMotors pointer to the right motors
         * @param trackWidth the track width of the robot, in inches. This is the distance from the left wheels to the
         * right wheels
         * @param wheelDiameter the diameter of the wheel used on the drivetrain, in inches
         * @param rpm the rpm of the wheels
         * @param horizontalDrift higher values make the robot move faster but causes more overshoot on turns.
         * Recommended value of 2 if not using traction wheels, 8 if using traction wheels
         *
         * @b Example
         * @code {.cpp}
         * // drive motors
         * pros::Motor lF(-5, pros::E_MOTOR_GEARSET_06); // left front motor. port 5, reversed
         * pros::Motor lM(4, pros::E_MOTOR_GEARSET_06); // left middle motor. port 4
         * pros::Motor lB(-3, pros::E_MOTOR_GEARSET_06); // left back motor. port 3, reversed
         * pros::Motor rF(6, pros::E_MOTOR_GEARSET_06); // right front motor. port 6
         * pros::Motor rM(-9, pros::E_MOTOR_GEARSET_06); // right middle motor. port 9, reversed
         * pros::Motor rB(7, pros::E_MOTOR_GEARSET_06); // right back motor. port 7
         *
         * // motor groups
         * pros::MotorGroup leftMotors({lF, lM, lB}); // left motor group
         * pros::MotorGroup rightMotors({rF, rM, rB}); // right motor group
         *
         * lemlib::Drivetrain drivetrain(&leftMotors, // left motor group
         *                               &rightMotors, // right motor group
         *                               10, // 10 inch track width
         *                               lemlib::Omniwheel::NEW_4, // using new 4" omnis
         *                               360, // drivetrain rpm is 360
         *                               2); // horizontalDrift is 2. If we had traction wheels, it would have been 8
         * @endcode
         */
        Drivetrain(pros::MotorGroup* leftMotors, pros::MotorGroup* rightMotors, float trackWidth, float wheelDiameter,
                   float rpm, float horizontalDrift);
        pros::MotorGroup* leftMotors;
        pros::MotorGroup* rightMotors;
        float trackWidth;
        float wheelDiameter;
        float rpm;
        float horizontalDrift;
};

/**
 * @brief AngularDirection
 *
 * When turning, the user may want to specify the direction the robot should turn in.
 * This enum class has 3 values: CW_CLOCKWISE, CCW_COUNTERCLOCKWISE, and AUTO
 * AUTO will make the robot turn in the shortest direction, and will be the most used value
 */
enum class AngularDirection {
    CW_CLOCKWISE, /** turn clockwise */
    CCW_COUNTERCLOCKWISE, /** turn counter-clockwise */
    AUTO /** turn in the direction with the shortest distance to target */
};

/**
 * @brief Parameters for Chassis::turnToPoint
 *
 * We use a struct to simplify customization. Chassis::turnToPoint has many
 * parameters and specifying them all just to set one optional param ruins
 * readability. By passing a struct to the function, we can have named
 * parameters, overcoming the c/c++ limitation
 */
struct TurnToPointParams {
        /** whether the robot should turn to face the point with the front of the robot. True by default */
        bool forwards = true;
        /** the direction the robot should turn in. AUTO by default */
        AngularDirection direction = AngularDirection::AUTO;
        /** the maximum speed the robot can turn at. Value between 0-127. 127 by default */
        int maxSpeed = 127;
        /** the minimum speed the robot can turn at. If set to a non-zero value, the `it conditions will switch to less
         * accurate but smoother ones. Value between 0-127. 0 by default */
        int minSpeed = 0;
        /** angle between the robot and target point where the movement will exit. Only has an effect if minSpeed is
         * non-zero.*/
        float earlyExitRange = 0;
};

/**
 * @brief Parameters for Chassis::turnToHeading
 *
 * We use a struct to simplify customization. Chassis::turnToHeading has many
 * parameters and specifying them all just to set one optional param ruins
 * readability. By passing a struct to the function, we can have named
 * parameters, overcoming the c/c++ limitation
 */
struct TurnToHeadingParams {
        /** the direction the robot should turn in. AUTO by default */
        AngularDirection direction = AngularDirection::AUTO;
        /** the maximum speed the robot can turn at. Value between 0-127. 127 by default */
        int maxSpeed = 127;
        /** the minimum speed the robot can turn at. If set to a non-zero value, the `it conditions will switch to less
         * accurate but smoother ones. Value between 0-127. 0 by default */
        int minSpeed = 0;
        /** angle between the robot and target point where the movement will exit. Only has an effect if minSpeed is
         * non-zero.*/
        float earlyExitRange = 0;
};

/**
 * @brief Enum class DriveSide
 *
 * When using swing turns, the user needs to specify what side of the drivetrain should be locked
 * we could just use an integer or boolean for this, but using an enum class improves readability
 *
 * This enum class only has 2 values, LEFT and RIGHT
 */
enum class DriveSide {
    LEFT, /** lock the left side of the drivetrain */
    RIGHT /** lock the right side of the drivetrain */
};

/**
 * @brief Parameters for Chassis::swingToPoint
 *
 * We use a struct to simplify customization. Chassis::swingToPoint has many
 * parameters and specifying them all just to set one optional param harms
 * readability. By passing a struct to the function, we can have named
 * parameters, overcoming the c/c++ limitation
 */
struct SwingToPointParams {
        /** whether the robot should turn to face the point with the front of the robot. True by default */
        bool forwards = true;
        /** the direction the robot should turn in. AUTO by default */
        AngularDirection direction = AngularDirection::AUTO;
        /** the maximum speed the robot can turn at. Value between 0-127. 127 by default */
        float maxSpeed = 127;
        /** the minimum speed the robot can turn at. If set to a non-zero value, the exit conditions will switch to less
         * accurate but smoother ones. Value between 0-127. 0 by default */
        float minSpeed = 0;
        /** angle between the robot and target heading where the movement will exit. Only has an effect if minSpeed is
         * non-zero.*/
        float earlyExitRange = 0;
};

/**
 * @brief Parameters for Chassis::swingToHeading
 *
 * We use a struct to simplify customization. Chassis::swingToHeading has many
 * parameters and specifying them all just to set one optional param harms
 * readability. By passing a struct to the function, we can have named
 * parameters, overcoming the c/c++ limitation
 */
struct SwingToHeadingParams {
        /** the direction the robot should turn in. AUTO by default */
        AngularDirection direction = AngularDirection::AUTO;
        /** the maximum speed the robot can turn at. Value between 0-127. 127 by default */
        float maxSpeed = 127;
        /** the minimum speed the robot can turn at. If set to a non-zero value, the exit conditions will switch to less
         * accurate but smoother ones. Value between 0-127. 0 by default */
        float minSpeed = 0;
        /** angle between the robot and target heading where the movement will exit. Only has an effect if minSpeed is
         * non-zero.*/
        float earlyExitRange = 0;
};

/**
 * @brief Parameters for Chassis::moveToPose
 *
 * We use a struct to simplify customization. Chassis::moveToPose has many
 * parameters and specifying them all just to set one optional param ruins
 * readability. By passing a struct to the function, we can have named
 * parameters, overcoming the c/c++ limitation
 */
struct MoveToPoseParams {
        /** whether the robot should move forwards or backwards. True by default */
        bool forwards = true;
        /** how fast the robot will move around corners. Recommended value 2-15. 0 means use horizontalDrift set in
         * chassis class. 0 by default. */
        float horizontalDrift = 0;
        /** carrot point multiplier. value between 0 and 1. Higher values result in curvier movements. 0.6 by default */
        float lead = 0.6;
        /** the maximum speed the robot can travel at. Value between 0-127. 127 by default */
        float maxSpeed = 127;
        /** the minimum speed the robot can travel at. If set to a non-zero value, the exit conditions will switch to
         * less accurate but smoother ones. Value between 0-127. 0 by default */
        float minSpeed = 0;
        /** distance between the robot and target point where the movement will exit. Only has an effect if minSpeed is
         * non-zero.*/
        float earlyExitRange = 0;
};

/**
 * @brief Parameters for Chassis::moveToPoint
 *
 * We use a struct to simplify customization. Chassis::moveToPoint has many
 * parameters and specifying them all just to set one optional param harms
 * readability. By passing a struct to the function, we can have named
 * parameters, overcoming the c/c++ limitation
 */
struct MoveToPointParams {
        /** whether the robot should move forwards or backwards. True by default */
        bool forwards = true;
        /** the maximum speed the robot can travel at. Value between 0-127. 127 by default */
        float maxSpeed = 127;
        /** the minimum speed the robot can travel at. If set to a non-zero value, the exit conditions will switch to
         * less accurate but smoother ones. Value between 0-127. 0 by default */
        float minSpeed = 0;
        /** distance between the robot and target point where the movement will exit. Only has an effect if minSpeed is
         * non-zero.*/
        float earlyExitRange = 0;
};

// default drive curve
extern ExpoDriveCurve defaultDriveCurve;

/**
 * @brief Chassis class
 */
class Chassis {
    public:
        /**
         * @brief Chassis constructor
         *
         * @param drivetrain drivetrain to be used for the chassis
         * @param lateralSettings settings for the lateral controller
         * @param angularSettings settings for the angular controller
         * @param sensors sensors to be used for odometry
         * @param throttleCurve curve applied to throttle input during driver control
         * @param turnCurve curve applied to steer input during driver control
         *
         * @example main.cpp
         */
        Chassis(Drivetrain drivetrain, ControllerSettings linearSettings, ControllerSettings angularSettings,
                OdomSensors sensors, DriveCurve* throttleCurve = &defaultDriveCurve,
                DriveCurve* steerCurve = &defaultDriveCurve);
        /**
         * @brief Calibrate the chassis sensors. THis should be called in the initialize function
         *
         * @param calibrateIMU whether the IMU should be calibrated. true by default
         *
         * @b Example
         * @code {.cpp}
         * // initialize function in your project. The first function that runs when the program is started
         * void initialize() {
         *     chassis.calibrate();
         * }
         * @endcode
         * @code {.cpp}
         * // initialize function in your project. The first function that runs when the program is started
         * void initialize() {
         *     // don't calibrate the IMU
         *     // this should only be necessary if you are using a different library that calibrates the IMU
         *     chassis.calibrate(false);
         * }
         * @endcode
         */
        void calibrate(bool calibrateIMU = true);
        /**
         * @brief Set the pose of the chassis
         *
         * @param x new x value
         * @param y new y value
         * @param theta new theta value
         * @param radians true if theta is in radians, false if not. False by default
         *
         * @b Example
         * @code {.cpp}
         * // set the pose of the chassis to x = 0, y = 0, theta = 0
         * chassis.setPose(0, 0, 0);
         * // set the pose of the chassis to x = 5.3, y = 12.2, theta = 3.14
         * // this time with theta in radians
         * chassis.setPose(5.3, 12.2, 3.14, true);
         * @endcode
         */
        void setPose(float x, float y, float theta, bool radians = false);
        /**
         * @brief Set the pose of the chassis
         *
         * @param pose the new pose
         * @param radians whether pose theta is in radians (true) or not (false). false by default
         *
         * @b Example
         * @code {.cpp}
         * // set the pose of the chassis to x = 0, y = 0, theta = 0
         * lemlib::Pose poseA(0, 0, 0);
         * chassis.setPose(poseA);
         * // set the pose of the chassis to x = 5.3, y = 12.2, theta = 3.14
         * // this time with theta in radians
         * lemlib::Pose poseB(5.3, 12.2, 3.14);
         * chassis.setPose(poseB, true);
         * @endcode
         */
        void setPose(Pose pose, bool radians = false);
        /**
         * @brief Get the pose of the chassis
         *
         * @param radians whether theta should be in radians (true) or degrees (false). false by default
         * @return Pose
         *
         * @b Example
         * @code {.cpp}
         * // get the pose of the chassis
         * lemlib::Pose pose = chassis.getPose();
         * // print the x, y, and theta values of the pose
         * printf("X: %f, Y: %f, Theta: %f\n", pose.x, pose.y, pose.theta);
         * // get the pose of the chassis in radians
         * lemlib::Pose poseRad = chassis.getPose(true);
         * // print the x, y, and theta values of the pose
         * printf("X: %f, Y: %f, Theta: %f\n", poseRad.x, poseRad.y, poseRad.theta);
         * // get the pose of the chassis in radians and standard position
         * lemlib::Pose poseRadStandard = chassis.getPose(true, true);
         * // print the x, y, and theta values of the pose
         * printf("X: %f, Y: %f, Theta: %f\n", poseRadStandard.x, poseRadStandard.y, poseRadStandard.theta);
         * @endcode
         */
        Pose getPose(bool radians = false, bool standardPos = false);
        /**
         * @brief Wait until the robot has traveled a certain distance along the path
         *
         * @note Units are in inches if current motion is moveToPoint, moveToPose or follow, degrees for everything else
         *
         * @param dist the distance the robot needs to travel before returning
         *
         * @b Example
         * @code {.cpp}
         * // move the robot to x = 20, y = 15, and face heading 90
         * chassis.moveToPose(20, 15, 90, 4000);
         * // wait until the robot has traveled 10 inches
         * chassis.waitUntil(10);
         * // output "traveled 10 inches" to the console
         * std::cout << "traveled 10 inches" << std::endl;
         * // turn the robot to face 270 degrees
         * // this will wait for the last motion to complete before running
         * chassis.turnToHeading(270, 4000);
         * // wait until the robot has traveled 45 degrees
         * chassis.waitUntil(45);
         * // output "traveled 45 degrees" to the console
         * std::cout << "traveled 45 degrees" << std::endl;
         * @endcode
         */
        void waitUntil(float dist);
        /**
         * @brief Wait until the robot has completed the path
         *
         * @b Example
         * @code {.cpp}
         * // move the robot to x = 20, y = 15, and face heading 90
         * chassis.moveToPose(20, 15, 90, 4000);
         * // wait until the robot has completed the motion
         * chassis.waitUntilDone();
         * // output "motion completed" to the console
         * std::cout << "motion completed" << std::endl;
         * @endcode
         */
        void waitUntilDone();
        /**
         * @brief Sets the brake mode of the drivetrain motors
         *
         * @param mode Mode to set the drivetrain motors to
         *
         * @b Example
         * @code {.cpp}
         * // set the brake mode of the drivetrain motors to hold
         * chassis.setBrakeMode(pros::E_MOTOR_BRAKE_HOLD);
         * // set the brake mode of the drivetrain motors to coast
         * chassis.setBrakeMode(pros::E_MOTOR_BRAKE_COAST);
         * // set the brake mode of the drivetrain motors to brake
         * chassis.setBrakeMode(pros::E_MOTOR_BRAKE_BRAKE);
         * @endcode
         */
        void setBrakeMode(pros::motor_brake_mode_e mode);
        /**
         * @brief Turn the chassis so it is facing the target point
         *
         * @param x x location
         * @param y y location
         * @param timeout longest time the robot can spend moving
         * @param params struct to simulate named parameters
         * @param async whether the function should be run asynchronously. true by default
         *
         * @b Example
         * @code {.cpp}
         * chassis.setPose(0, 0, 0); // set the pose of the chassis to x = 0, y = 0, theta = 0
         * // turn the robot to face the point x = 45, y = -45, with a timeout of 1000ms
         * chassis.turnToPoint(45, -45, 1000);
         * // turn the robot to face the point x = 45, y = -45, with a timeout of 1000ms
         * // but face the point with the back of the robot
         * chassis.turnToPoint(45, -45, 1000, {.forwards = false});
         * // turn the robot to face the point x = -20, 32.5 with a timeout of 2000ms
         * // and a maximum speed of 60
         * chassis.turnToPoint(-20, 32.5, 2000, {.maxSpeed = 60});
         * // turn the robot to face the point x = -30, y = 22.5 with a timeout of 1500ms
         * // and turn counterclockwise
         * chassis.turnToPoint(-30, 22.5, 1500, {.direction = AngularDirection::CCW_COUNTERCLOCKWISE});
         * // turn the robot to face the point x = 10, y = 10 with a timeout of 500ms
         * // with a minSpeed of 20 and a maxSpeed of 60
         * chassis.turnToPoint(10, 10, 500, {.maxSpeed = 60, .minSpeed = 20});
         * // turn the robot to face the point x = 7.5, y = 7.5 with a timeout of 2000ms
         * // and a minSpeed of 60, and exit the movement if the robot is within 5 degrees of the target
         * chassis.turnToPoint(7.5, 7.5, 2000, {.minSpeed = 60, .earlyExitRange = 5});
         * @endcode
         */
        void turnToPoint(float x, float y, int timeout, TurnToPointParams params = {}, bool async = true);
        /**
         * @brief Turn the chassis so it is facing the target heading
         *
         * @param theta heading location
         * @param timeout longest time the robot can spend moving
         * @param params struct to simulate named parameters
         * @param async whether the function should be run asynchronously. true by default
         *
         * @b Example
         * @code {.cpp}
         * chassis.setPose(0, 0, 0); // set the pose of the chassis to x = 0, y = 0, theta = 0
         * // turn the robot to face heading 135, with a timeout of 1000ms
         * chassis.turnToHeading(135, 1000);
         * // turn the robot to face heading 230.5 with a timeout of 2000ms
         * // and a maximum speed of 60
         * chassis.turnToHeading(230.5, 2000, {.maxSpeed = 60});
         * // turn the robot to face heading -90 with a timeout of 1500ms
         * // and turn counterclockwise
         * chassis.turnToHeading(-90, 1500, {.direction = AngularDirection::CCW_COUNTERCLOCKWISE});
         * // turn the robot to face heading 90 with a timeout of 500ms
         * // with a minSpeed of 20 and a maxSpeed of 60
         * chassis.turnToHeading(90, 500, {.maxSpeed = 60, .minSpeed = 20});
         * // turn the robot to face heading 45 with a timeout of 2000ms
         * // and a minSpeed of 60, and exit the movement if the robot is within 5 degrees of the target
         * chassis.turnToHeading(45, 2000, {.minSpeed = 60, .earlyExitRange = 5});
         * @endcode
         */
        void turnToHeading(float theta, int timeout, TurnToHeadingParams params = {}, bool async = true);
        /**
         * @brief Turn the chassis so it is facing the target heading, but only by moving one half of the drivetrain
         *
         * @param theta heading location
         * @param lockedSide side of the drivetrain that is locked
         * @param timeout longest time the robot can spend moving
         * @param params struct to simulate named parameters
         * @param async whether the function should be run asynchronously. true by default
         *
         * @b Example
         * @code {.cpp}
         * chassis.setPose(0, 0, 0); // set the pose of the chassis to x = 0, y = 0, theta = 0
         * // turn the robot to face heading 135, with a timeout of 1000ms
         * // and lock the left side of the drivetrain
         * chassis.swingToHeading(135, DriveSide::LEFT, 1000);
         * // turn the robot to face heading 230.5 with a timeout of 2000ms
         * // and a maximum speed of 60
         * // and lock the right side of the drivetrain
         * chassis.swingToHeading(230.5, DriveSide::RIGHT, 2000, {.maxSpeed = 60});
         * // turn the robot to face heading -90 with a timeout of 1500ms
         * // and turn counterclockwise
         * // and lock the left side of the drivetrain
         * chassis.swingToHeading(-90, DriveSide::LEFT, 1500, {.direction = AngularDirection::CCW_COUNTERCLOCKWISE});
         * // turn the robot to face heading 90 with a timeout of 500ms
         * // with a minSpeed of 20 and a maxSpeed of 60
         * // and lock the right side of the drivetrain
         * chassis.swingToHeading(90, DriveSide::RIGHT, 500, {.maxSpeed = 60, .minSpeed = 20});
         * // turn the robot to face heading 45 with a timeout of 2000ms
         * // and a minSpeed of 60, and exit the movement if the robot is within 5 degrees of the target
         * // and lock the left side of the drivetrain
         * chassis.swingToHeading(45, DriveSide::LEFT, 2000, {.minSpeed = 60, .earlyExitRange = 5});
         * @endcode
         */
        void swingToHeading(float theta, DriveSide lockedSide, int timeout, SwingToHeadingParams params = {},
                            bool async = true);
        /**
         * @brief Turn the chassis so it is facing the target point, but only by moving one half of the drivetrain
         *
         * @param x x location
         * @param y y location
         * @param lockedSide side of the drivetrain that is locked
         * @param timeout longest time the robot can spend moving
         * @param params struct to simulate named parameters
         * @param async whether the function should be run asynchronously. true by default
         *
         * @b Example
         * @code {.cpp}
         * chassis.setPose(0, 0, 0); // set the pose of the chassis to x = 0, y = 0, theta = 0
         * // turn the robot to face the point x = 45, y = -45, with a timeout of 1000ms
         * // and lock the left side of the drivetrain
         * chassis.swingToPoint(45, -45, DriveSide::LEFT, 1000);
         * // turn the robot to face the point x = 45, y = -45, with a timeout of 1000ms
         * // but face the point with the back of the robot
         * // and lock the right side of the drivetrain
         * chassis.swingToPoint(45, -45, DriveSide::RIGHT, 1000, {.forwards = false});
         * // turn the robot to face the point x = -20, 32.5 with a timeout of 2000ms
         * // and a maximum speed of 60
         * // and lock the left side of the drivetrain
         * chassis.swingToPoint(-20, 32.5, DriveSide::LEFT, 2000, {.maxSpeed = 60});
         * // turn the robot to face the point x = -30, y = 22.5 with a timeout of 1500ms
         * // and turn counterclockwise
         * // and lock the right side of the drivetrain
         * chassis.swingToPoint(-30, 22.5, DriveSide::RIGHT, 1500, {.direction =
         * AngularDirection::CCW_COUNTERCLOCKWISE});
         * // turn the robot to face the point x = 10, y = 10 with a timeout of 500ms
         * // with a minSpeed of 20 and a maxSpeed of 60
         * // and lock the left side of the drivetrain
         * chassis.swingToPoint(10, 10, DriveSide::LEFT, 500, {.maxSpeed = 60, .minSpeed = 20});
         * // turn the robot to face the point x = 7.5, y = 7.5 with a timeout of 2000ms
         * // and a minSpeed of 60, and exit the movement if the robot is within 5 degrees of the target
         * // and lock the right side of the drivetrain
         * chassis.swingToPoint(7.5, 7.5, DriveSide::RIGHT, 2000, {.minSpeed = 60, .earlyExitRange = 5});
         * @endcode
         */
        void swingToPoint(float x, float y, DriveSide lockedSide, int timeout, SwingToPointParams params = {},
                          bool async = true);
        /**
         * @brief Move the chassis towards the target pose
         *
         * Uses the boomerang controller
         *
         * @param x x location
         * @param y y location
         * @param theta target heading in degrees.
         * @param timeout longest time the robot can spend moving
         * @param params struct to simulate named parameters
         * @param async whether the function should be run asynchronously. true by default
         *
         * @b Example
         * @code {.cpp}
         * // move the robot to x = 20, y = 15, and face heading 90 with a timeout of 4000ms
         * chassis.moveToPose(20, 15, 90, 4000);
         * // move the robot to x = 20, y = 15, and face heading 90 with a timeout of 4000ms
         * // but face the point with the back of the robot
         * chassis.moveToPose(20, 15, 90, 4000, {.forwards = false});
         * // move the robot to x = -20, 32.5 and face heading 90 with a timeout of 4000ms
         * // with a maxSpeed of 60
         * chassis.moveToPose(-20, 32.5, 90, 4000, {.maxSpeed = 60});
         * // move the robot to x = 10, y = 10 and face heading 90
         * // with a minSpeed of 20 and a maxSpeed of 60
         * chassis.moveToPose(10, 10, 90, 4000, {.maxSpeed = 60, .minSpeed = 20});
         * // move the robot to x = 7.5, y = 7.5 and face heading 90 with a timeout of 4000ms
         * // with a minSpeed of 60, and exit the movement if the robot is within 5 inches of the target
         * chassis.moveToPose(7.5, 7.5, 90, 4000, {.minSpeed = 60, .earlyExitRange = 5});
         * // move the robot to 0, 0, and facing heading 0 with a timeout of 4000ms
         * // this motion should not be as curved as the others, so we set lead to a smaller value (0.3)
         * chassis.moveToPose(0, 0, 0, 4000, {.lead = 0.3});
         * @endcode
         */
        void moveToPose(float x, float y, float theta, int timeout, MoveToPoseParams params = {}, bool async = true);
        /**
         * @brief Move the chassis towards a target point
         *
         * @param x x location
         * @param y y location
         * @param timeout longest time the robot can spend moving
         * @param params struct to simulate named parameters
         * @param async whether the function should be run asynchronously. true by default
         *
         * @b Example
         * @code {.cpp}
         * // move the robot to x = 20, y = 15 with a timeout of 4000ms
         * chassis.moveToPoint(20, 15, 4000);
         * // move the robot to x = 20, y = 15 with a timeout of 4000ms
         * // but face the point with the back of the robot
         * chassis.moveToPoint(20, 15, 4000, {.forwards = false});
         * // move the robot to x = -20, 32.5 with a timeout of 4000ms
         * // with a maxSpeed of 60
         * chassis.moveToPoint(-20, 32.5, 4000, {.maxSpeed = 60});
         * // move the robot to x = 10, y = 10 with a timeout of 4000ms
         * // with a minSpeed of 20 and a maxSpeed of 60
         * chassis.moveToPoint(10, 10, 4000, {.maxSpeed = 60, .minSpeed = 20});
         * // move the robot to x = 7.5, y = 7.5 with a timeout of 4000ms
         * // with a minSpeed of 60, and exit the movement if the robot is within 5 inches of the target
         * chassis.moveToPoint(7.5, 7.5, 4000, {.minSpeed = 60, .earlyExitRange = 5});
         * @endcode
         */
        void moveToPoint(float x, float y, int timeout, MoveToPointParams params = {}, bool async = true);
        /**
         * @brief Move the chassis along a path
         *
         * @param path the path asset to follow
         * @param lookahead the lookahead distance. Units in inches. Larger values will make the robot move
         * faster but will follow the path less accurately
         * @param timeout the maximum time the robot can spend moving
         * @param forwards whether the robot should follow the path going forwards. true by default
         * @param async whether the function should be run asynchronously. true by default
         *
         * @b Example
         * @code {.cpp}
         * // load "myPath.txt"
         * // the file should be in the "static" folder in the project root directory
         * // this should also be done outside of any functions, otherwise it won't compile
         * ASSET(myPath_txt); // we replace "." with "_" to make the asset name valid
         *
         * // autonomous function in your project. The function that runs during the autonomous period
         * void autonomous() {
         *     // follow the path in "myPath.txt" with a lookahead of 10 inches and a timeout of 4000ms
         *     chassis.follow(myPath_txt, 10, 4000);
         *     // follow the path in "myPath.txt" with a lookahead of 10 inches and a timeout of 4000ms
         *     // but follow the path backwards
         *     chassis.follow(myPath_txt, 10, 4000, false);
         * }
         * @endcode
         */
        void follow(const asset& path, float lookahead, int timeout, bool forwards = true, bool async = true);
        /**
         * @brief Control the robot during the driver using the tank drive control scheme. In this control scheme one
         * joystick axis controls the left motors' forward and backwards movement of the robot, while the other joystick
         * axis controls right motors' forward and backward movement.
         * @param left speed to move left wheels forward or backward. Takes an input from -127 to 127.
         * @param right speed to move right wheels forward or backward. Takes an input from -127 to 127.
         * @param disableDriveCurve whether to disable the drive curve or not. If disabled, uses a linear curve with no
         * deadzone or minimum power
         *
         * @b Example
         * @code {.cpp}
         * // opcontrol function in your project. The function that runs during the driver control period
         * void opcontrol() {
         *     // controller
         *     pros::Controller controller(pros::E_CONTROLLER_MASTER);
         *     // loop to continuously update motors
         *     while (true) {
         *         // get joystick positions
         *         int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
         *         int rightY = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_Y);
         *         // move the chassis with tank controls
         *         chassis.tank(leftY, rightY);
         *         // delay to save resources
         *         pros::delay(25);
         *     }
         * }
         * @endcode
         */
        void tank(int left, int right, bool disableDriveCurve = false);
        /**
         * @brief Control the robot during the driver using the arcade drive control scheme. In this control scheme one
         * joystick axis controls the forwards and backwards movement of the robot, while the other joystick axis
         * controls the robot's turning
         *
         * @param throttle speed to move forward or backward. Takes an input from -127 to 127.
         * @param turn speed to turn. Takes an input from -127 to 127.
         * @param disableDriveCurve whether to disable the drive curve or not. If disabled, uses a linear curve with no
         * deadzone or minimum power
         * @param desaturateBias how much to favor angular motion over lateral motion or vice versa when motors are
         * saturated. A value of 0 fully prioritizes lateral motion, a value of 1 fully prioritizes angular motion
         *
         * @b Example
         * @code {.cpp}
         * // opcontrol function in your project. The function that runs during the driver control period
         * void opcontrol() {
         *     // controller
         *     pros::Controller controller(pros::E_CONTROLLER_MASTER);
         *     // loop to continuously update motors
         *     while (true) {
         *         // get joystick positions
         *         int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
         *         int rightX = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
         *         // move the chassis with arcade controls
         *         chassis.arcade(leftY, // throttle
         *                        rightX, // steer
         *                        false, // enable drive curves
         *                        0.75 // slightly prioritize steering
         *         );
         *         // delay to save resources
         *         pros::delay(25);
         *     }
         * }
         * @endcode
         */
        void arcade(int throttle, int turn, bool disableDriveCurve = false, float desaturateBias = 0.5);
        /**
         * @brief Control the robot during the driver using the curvature drive control scheme. This control scheme is
         * very similar to arcade drive, except the second joystick axis controls the radius of the curve that the
         * drivetrain makes, rather than the speed. This means that the driver can accelerate in a turn without changing
         * the radius of that turn. This control scheme defaults to arcade when forward is zero.
         *
         * @param throttle speed to move forward or backward. Takes an input from -127 to 127.
         * @param turn speed to turn. Takes an input from -127 to 127.
         * @param disableDriveCurve whether to disable the drive curve or not. If disabled, uses a linear curve with no
         * deadzone or minimum power
         *
         * @b Example
         * @code {.cpp}
         * // opcontrol function in your project. The function that runs during the driver control period
         * void opcontrol() {
         *     // controller
         *     pros::Controller controller(pros::E_CONTROLLER_MASTER);
         *     // loop to continuously update motors
         *     while (true) {
         *         // get joystick positions
         *         int leftY = controller.get_analog(pros::E_CONTROLLER_ANALOG_LEFT_Y);
         *         int rightX = controller.get_analog(pros::E_CONTROLLER_ANALOG_RIGHT_X);
         *         // move the chassis with curvature controls
         *         chassis.curvature(leftY, rightX);
         *         // delay to save resources
         *         pros::delay(25);
         *     }
         * }
         * @endcode
         */
        void curvature(int throttle, int turn, bool disableDriveCurve = false);
        /**
         * @brief Cancels the currently running motion.
         * If there is a queued motion, then that queued motion will run.
         *
         * @b Example
         * @code {.cpp}
         * // move the robot to x = 20, y = 20 with a timeout of 4000ms
         * chassis.moveToPoint(20, 20, 4000);
         * // wait 500 milliseconds
         * pros::delay(500);
         * // cancel the current motion. This stops it immediately
         * chassis.cancelMotion();
         * @endcode
         * @b Example (advanced)
         * @code {.cpp}
         * // this example shows how the cancelMotion function behaves when a motion is queued
         * // this is an advanced example since we will be using tasks here
         * // move the robot to x = 20, y = 20 with a timeout of 4000ms
         * chassis.moveToPoint(20, 20, 4000);
         * // start a lambda task that will be used to cancel the motion after 500ms
         * pros::Task task([] {
         *     // wait 500 milliseconds
         *     pros::delay(500);
         *     // cancel the current motion. This stops it immediately
         *     chassis.cancelMotion();
         * });
         * // queue a motion to x = 10, y = 10 with a timeout of 4000ms
         * // this will run after the first motion is cancelled
         * chassis.moveToPoint(10, 10, 4000);
         * @endcode
         */
        void cancelMotion();
        /**
         * @brief Cancels all motions, even those that are queued.
         * After this, the chassis will not be in motion.
         *
         * @b Example
         * @code {.cpp}
         * // move the robot to x = 20, y = 20 with a timeout of 4000ms
         * chassis.moveToPoint(20, 20, 4000);
         * // wait 500 milliseconds
         * pros::delay(500);
         * // cancel all motions. The robot will stop immediately
         * chassis.cancelAllMotions();
         * @endcode
         * @b Example (advanced)
         * @code {.cpp}
         * // this example shows how the cancelMotion function behaves when a motion is queued
         * // this is an advanced example since we will be using tasks here
         * // move the robot to x = 20, y = 20 with a timeout of 4000ms
         * chassis.moveToPoint(20, 20, 4000);
         * // start a lambda task that will be used to cancel all motions after 500ms
         * pros::Task task([] {
         *     // wait 500 milliseconds
         *     pros::delay(500);
         *     // cancels both motions
         *     chassis.cancelAllMotions();
         * });
         * // queue a motion to x = 10, y = 10 with a timeout of 4000ms
         * // this will never run because cancelAllMotions will be called while this motion is in the queue
         * chassis.moveToPoint(10, 10, 4000);
         * @endcode
         */
        void cancelAllMotions();
        /**
         * @return whether a motion is currently running
         *
         * @b Example
         * @code {.cpp}
         * // move the robot to x = 20, y = 15, and face heading 90
         * chassis.moveToPose(20, 15, 90, 4000);
         * // delay for 500ms
         * // this returns true, since the robot is still in motion
         * chassis.isInMotion();
         * @endcode
         */
        bool isInMotion() const;
        /**
         * @brief Resets the x and y position of the robot
         * without interfering with the heading.
         *
         * @b Example
         * @code {.cpp}
         * // set robot position to x = 10, y = 15, and heading 90
         * chassis.setPose(10, 15, 90);
         * // reset the robot's x and y position
         * chassis.resetLocalPosition();
         * // the robot's position is now x = 0, y = 0, and heading 90
         * @endcode
         */
        void resetLocalPosition();
        /**
         * PIDs are exposed so advanced users can implement things like gain scheduling
         * Changes are immediate and will affect a motion in progress
         *
         * @warning Do not interact with these unless you know what you are doing
         */
        PID lateralPID;
        /**
         * PIDs are exposed so advanced users can implement things like gain scheduling
         * Changes are immediate and will affect a motion in progress
         *
         * @warning Do not interact with these unless you know what you are doing
         */
        PID angularPID;
    protected:
        /**
         * @brief Indicates that this motion is queued and blocks current task until this motion reaches front of queue
         */
        void requestMotionStart();
        /**
         * @brief Dequeues this motion and permits queued task to run
         */
        void endMotion();

        bool motionRunning = false;
        bool motionQueued = false;

        float distTraveled = 0;

        ControllerSettings lateralSettings;
        ControllerSettings angularSettings;
        Drivetrain drivetrain;
        OdomSensors sensors;
        DriveCurve* throttleCurve;
        DriveCurve* steerCurve;

        ExitCondition lateralLargeExit;
        ExitCondition lateralSmallExit;
        ExitCondition angularLargeExit;
        ExitCondition angularSmallExit;
    private:
        pros::Mutex mutex;
};
} // namespace lemlib
