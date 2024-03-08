#pragma once

#include <functional>
#include "pros/rtos.hpp"
#include "pros/motors.hpp"
#include "pros/imu.hpp"
#include "lemlib/asset.hpp"
#include "lemlib/chassis/trackingWheel.hpp"
#include "lemlib/pose.hpp"
#include "lemlib/pid.hpp"
#include "lemlib/exitcondition.hpp"

namespace lemlib {
/**
 * @brief Struct containing all the sensors used for odometry
 *
 */
struct OdomSensors {
        /**
         * The sensors are stored in a struct so that they can be easily passed to the chassis class
         * The variables are pointers so that they can be set to nullptr if they are not used
         * Otherwise the chassis class would have to have a constructor for each possible combination of sensors
         *
         * @param vertical1 pointer to the first vertical tracking wheel
         * @param vertical2 pointer to the second vertical tracking wheel
         * @param horizontal1 pointer to the first horizontal tracking wheel
         * @param horizontal2 pointer to the second horizontal tracking wheel
         * @param imu pointer to the IMU
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
 * @brief Struct containing constants for a chassis controller
 *
 */
struct ControllerSettings {
        /**
         * The constants are stored in a struct so that they can be easily passed to the chassis class
         * Set a constant to 0 and it will be ignored
         *
         * @param kP proportional constant for the chassis controller
         * @param kI integral constant for the chassis controller
         * @param kD derivative constant for the chassis controller
         * @param antiWindup
         * @param smallError the error at which the chassis controller will switch to a slower control loop
         * @param smallErrorTimeout the time the chassis controller will wait before switching to a slower control loop
         * @param largeError the error at which the chassis controller will switch to a faster control loop
         * @param largeErrorTimeout the time the chassis controller will wait before switching to a faster control loop
         * @param slew the maximum acceleration of the chassis controller
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
 * @brief Struct containing constants for a drivetrain
 *
 */
struct Drivetrain {
        /**
         * The constants are stored in a struct so that they can be easily passed to the chassis class
         * Set a constant to 0 and it will be ignored
         *
         * @param leftMotors pointer to the left motors
         * @param rightMotors pointer to the right motors
         * @param trackWidth the track width of the robot
         * @param wheelDiameter the diameter of the wheel used on the drivetrain
         * @param rpm the rpm of the wheels
         * @param chasePower higher values make the robot move faster but causes more overshoot on turns
         */
        Drivetrain(pros::MotorGroup* leftMotors, pros::MotorGroup* rightMotors, float trackWidth, float wheelDiameter,
                   float rpm, float chasePower);
        pros::Motor_Group* leftMotors;
        pros::Motor_Group* rightMotors;
        float trackWidth;
        float wheelDiameter;
        float rpm;
        float chasePower;
};

/**
 * @brief Parameters for Chassis::turnTo
 *
 * We use a struct to simplify customization. Chassis::turnTo has many
 * parameters and specifying them all just to set one optional param ruins
 * readability. By passing a struct to the function, we can have named
 * parameters, overcoming the c/c++ limitation
 *
 * @param forwards whether the robot should turn to face the point with the front of the robot.
 * True by default
 * @param maxSpeed the maximum speed the robot can turn at. Value between 0-127.
 *  127 by default
 * @param minSpeed the minimum speed the robot can turn at. If set to a non-zero value,
 *  the exit conditions will switch to less accurate but smoother ones. Value between 0-127.
 *  0 by default
 * @param earlyExitRange angle between the robot and target point where the movement will
 *  exit. Only has an effect if minSpeed is non-zero.
 */
struct TurnToParams {
        bool forwards = true;
        int maxSpeed = 127;
        int minSpeed = 0;
        float earlyExitRange = 0;
};

/**
 * @brief Parameters for Chassis::moveToPose
 *
 * We use a struct to simplify customization. Chassis::moveToPose has many
 * parameters and specifying them all just to set one optional param ruins
 * readability. By passing a struct to the function, we can have named
 * parameters, overcoming the c/c++ limitation
 *
 * @param forwards whether the robot should move forwards or backwards. True by default
 * @param chasePower how fast the robot will move around corners. Recommended value 2-15.
 *  0 means use chasePower set in chassis class. 0 by default.
 * @param lead carrot point multiplier. value between 0 and 1. Higher values result in
 *  curvier movements. 0.6 by default
 * @param maxSpeed the maximum speed the robot can travel at. Value between 0-127.
 *  127 by default
 * @param minSpeed the minimum speed the robot can travel at. If set to a non-zero value,
 *  the exit conditions will switch to less accurate but smoother ones. Value between 0-127.
 *  0 by default
 * @param earlyExitRange distance between the robot and target point where the movement will
 *  exit. Only has an effect if minSpeed is non-zero.
 */
struct MoveToPoseParams {
        bool forwards = true;
        float chasePower = 0;
        float lead = 0.6;
        float maxSpeed = 127;
        float minSpeed = 0;
        float earlyExitRange = 0;
};

/**
 * @brief Parameters for Chassis::moveToPoint
 *
 * We use a struct to simplify customization. Chassis::moveToPoint has many
 * parameters and specifying them all just to set one optional param harms
 * readability. By passing a struct to the function, we can have named
 * parameters, overcoming the c/c++ limitation
 *
 * @param forwards whether the robot should move forwards or backwards. True by default
 * @param maxSpeed the maximum speed the robot can travel at. Value between 0-127.
 *  127 by default
 * @param minSpeed the minimum speed the robot can travel at. If set to a non-zero value,
 *  the exit conditions will switch to less accurate but smoother ones. Value between 0-127.
 *  0 by default
 * @param earlyExitRange distance between the robot and target point where the movement will
 *  exit. Only has an effect if minSpeed is non-zero.
 */
struct MoveToPointParams {
        bool forwards = true;
        float maxSpeed = 127;
        float minSpeed = 0;
        float earlyExitRange = 0;
};

/**
 * @brief Function pointer type for drive curve functions.
 * @param input The control input in the range [-127, 127].
 * @param scale The scaling factor, which can be optionally ignored.
 * @return The new value to be used.
 */
typedef std::function<float(float, float)> DriveCurveFunction_t;

/**
 * @brief  Default drive curve. Modifies  the input with an exponential curve. If the input is 127, the function
 * will always output 127, no matter the value of scale, likewise for -127. This curve was inspired by team
 * 5225, the Pilons. A Desmos graph of this curve can be found here:
 * https://www.desmos.com/calculator/rcfjjg83zx
 * @param input value from -127 to 127
 * @param scale how steep the curve should be.
 * @return The new value to be used.
 */
float defaultDriveCurve(float input, float scale);

/**
 * @brief Chassis class
 *
 */
class Chassis {
    public:
        /**
         * @brief Construct a new Chassis
         *
         * @param drivetrain drivetrain to be used for the chassis
         * @param lateralSettings settings for the lateral controller
         * @param angularSettings settings for the angular controller
         * @param sensors sensors to be used for odometry
         * @param driveCurve drive curve to be used. defaults to `defaultDriveCurve`
         */
        Chassis(Drivetrain drivetrain, ControllerSettings linearSettings, ControllerSettings angularSettings,
                OdomSensors sensors, DriveCurveFunction_t driveCurve = &defaultDriveCurve);
        /**
         * @brief Calibrate the chassis sensors
         *
         * @param calibrateIMU whether the IMU should be calibrated. true by default
         */
        void calibrate(bool calibrateIMU = true);
        /**
         * @brief Set the pose of the chassis
         *
         * @param x new x value
         * @param y new y value
         * @param theta new theta value
         * @param radians true if theta is in radians, false if not. False by default
         */
        void setPose(float x, float y, float theta, bool radians = false);
        /**
         * @brief Set the pose of the chassis
         *
         * @param pose the new pose
         * @param radians whether pose theta is in radians (true) or not (false). false by default
         */
        void setPose(Pose pose, bool radians = false);
        /**
         * @brief Get the pose of the chassis
         *
         * @param radians whether theta should be in radians (true) or degrees (false). false by default
         * @return Pose
         */
        Pose getPose(bool radians = false, bool standardPos = false);
        /**
         * @brief Wait until the robot has traveled a certain distance along the path
         *
         * @note Units are in inches if current motion is moveTo or follow, degrees if using turnTo
         *
         * @param dist the distance the robot needs to travel before returning
         */
        void waitUntil(float dist);
        /**
         * @brief Wait until the robot has completed the path
         *
         */
        void waitUntilDone();
        /**
         * @brief Sets the brake mode of the drivetrain motors
         *
         * @param mode Mode to set the drivetrain motors to
         */
        void setBrakeMode(pros::motor_brake_mode_e mode);
        /**
         * @brief Turn the chassis so it is facing the target point
         *
         * The PID logging id is "angularPID"
         *
         * @param x x location
         * @param y y location
         * @param timeout longest time the robot can spend moving
         * @param async whether the function should be run asynchronously. true by default
         */
        void turnToPoint(float x, float y, int timeout, bool async = true);
        /**
         * @brief Turn the chassis so it is facing the target point
         *
         * The PID logging id is "angularPID"
         *
         * @param x x location
         * @param y y location
         * @param timeout longest time the robot can spend moving
         * @param params struct to simulate named parameters
         * @param async whether the function should be run asynchronously. true by default
         */
        void turnToPoint(float x, float y, int timeout, TurnToParams params, bool async = true);
        /**
         * @brief Turn the chassis so it is facing the target heading
         *
         * The PID logging id is "angularPID"
         *
         * @param theta heading location
         * @param timeout longest time the robot can spend moving
         * @param params struct to simulate named parameters
         * @param async whether the function should be run asynchronously. true by default
         */
        void turnToHeading(float theta, int timeout, bool async = true);
        /**
         * @brief Turn the chassis so it is facing the target heading
         *
         * The PID logging id is "angularPID"
         *
         * @param theta heading location
         * @param timeout longest time the robot can spend moving
         * @param params struct to simulate named parameters
         * @param async whether the function should be run asynchronously. true by default
         */
        void turnToHeading(float theta, int timeout, TurnToParams params, bool async = true);
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
         */
        void follow(const asset& path, float lookahead, int timeout, bool forwards = true, bool async = true);
        /**
         * @brief Control the robot during the driver control period using the tank drive control scheme. In
         * this control scheme one joystick axis controls one half of the robot, and another joystick axis
         * controls another.
         * @param left speed of the left side of the drivetrain. Takes an input from -127 to 127.
         * @param right speed of the right side of the drivetrain. Takes an input from -127 to 127.
         * @param curveGain control how steep the drive curve is. The larger the number, the steeper the curve.
         * A value of 0 disables the curve entirely.
         */
        void tank(int left, int right, float curveGain = 0.0);
        /**
         * @brief Control the robot during the driver using the arcade drive control scheme. In this control
         * scheme one joystick axis controls the forwards and backwards movement of the robot, while the other
         * joystick axis controls  the robot's turning
         * @param throttle speed to move forward or backward. Takes an input from -127 to 127.
         * @param turn speed to turn. Takes an input from -127 to 127.
         * @param curveGain the scale inputted into the drive curve function. If you are using the default drive
         * curve, refer to the `defaultDriveCurve` documentation.
         */
        void arcade(int throttle, int turn, float curveGain = 0.0);
        /**
         * @brief Control the robot during the driver using the curvature drive control scheme. This control
         * scheme is very similar to arcade drive, except the second joystick axis controls the radius of the
         * curve that the drivetrain makes, rather than the speed. This means that the driver can accelerate in
         * a turn without changing the radius of that turn. This control scheme defaults to arcade when forward
         * is zero.
         * @param throttle speed to move forward or backward. Takes an input from -127 to 127.
         * @param turn speed to turn. Takes an input from -127 to 127.
         * @param curveGain the scale inputted into the drive curve function. If you are using the default drive
         * curve, refer to the `defaultDriveCurve` documentation.
         */
        void curvature(int throttle, int turn, float cureGain = 0.0);
        /**
         * @brief Cancels the currently running motion.
         * If there is a queued motion, then that queued motion will run.
         */
        void cancelMotion();
        /**
         * @brief Cancels all motions, even those that are queued.
         * After this, the chassis will not be in motion.
         */
        void cancelAllMotions();
        /**
         * @return whether a motion is currently running
         */
        bool isInMotion() const;
        /**
         * @brief Resets the x and y position of the robot
         * without interfering with the heading.
         */
        void resetLocalPosition();
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

        float distTravelled = 0;

        ControllerSettings lateralSettings;
        ControllerSettings angularSettings;
        Drivetrain drivetrain;
        OdomSensors sensors;
        DriveCurveFunction_t driveCurve;

        PID lateralPID;
        PID angularPID;
        ExitCondition lateralLargeExit;
        ExitCondition lateralSmallExit;
        ExitCondition angularLargeExit;
        ExitCondition angularSmallExit;
    private:
        pros::Mutex mutex;
};
} // namespace lemlib
