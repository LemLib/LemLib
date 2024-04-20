#pragma once

#include "pros/rtos.hpp"
#include "pros/motors.hpp"
#include "pros/imu.hpp"
#include "lemlib/asset.hpp"
#include "lemlib/chassis/trackingWheel.hpp"
#include "lemlib/pose.hpp"
#include "lemlib/pid.hpp"
#include "lemlib/exitcondition.hpp"
#include "lemlib/driveCurve.hpp"

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
 * @brief AngularDirection
 *
 * When turning, the user may want to specify the direction the robot should turn in.
 * This enum class has 3 values: CW_CLOCKWISE, CCW_COUNTERCLOCKWISE, and AUTO
 * AUTO will make the robot turn in the shortest direction, and will be the most used value
 */
enum class AngularDirection { CW_CLOCKWISE, CCW_COUNTERCLOCKWISE, AUTO };

/**
 * @brief Parameters for Chassis::turnToPoint
 *
 * We use a struct to simplify customization. Chassis::turnToPoint has many
 * parameters and specifying them all just to set one optional param ruins
 * readability. By passing a struct to the function, we can have named
 * parameters, overcoming the c/c++ limitation
 *
 * @param forwards whether the robot should turn to face the point with the front of the robot.
 * True by default
 * @param direction the direction the robot should turn in. AUTO by default
 * @param maxSpeed the maximum speed the robot can turn at. Value between 0-127.
 *  127 by default
 * @param minSpeed the minimum speed the robot can turn at. If set to a non-zero value,
 *  the `it conditions will switch to less accurate but smoother ones. Value between 0-127.
 *  0 by default
 * @param earlyExitRange angle between the robot and target point where the movement will
 *  exit. Only has an effect if minSpeed is non-zero.
 */
struct TurnToPointParams {
        bool forwards = true;
        AngularDirection direction = AngularDirection::AUTO;
        int maxSpeed = 127;
        int minSpeed = 0;
        float earlyExitRange = 0;
};

/**
 * @brief Parameters for Chassis::turnToHeading
 *
 * We use a struct to simplify customization. Chassis::turnToHeading has many
 * parameters and specifying them all just to set one optional param ruins
 * readability. By passing a struct to the function, we can have named
 * parameters, overcoming the c/c++ limitation
 *
 * @param direction the direction the robot should turn in. AUTO by default
 * @param maxSpeed the maximum speed the robot can turn at. Value between 0-127.
 *  127 by default
 * @param minSpeed the minimum speed the robot can turn at. If set to a non-zero value,
 *  the `it conditions will switch to less accurate but smoother ones. Value between 0-127.
 *  0 by default
 * @param earlyExitRange angle between the robot and target point where the movement will
 *  exit. Only has an effect if minSpeed is non-zero.
 */
struct TurnToHeadingParams {
        AngularDirection direction = AngularDirection::AUTO;
        int maxSpeed = 127;
        int minSpeed = 0;
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
enum class DriveSide { LEFT, RIGHT };

/**
 * @brief Parameters for Chassis::swingToPoint
 *
 * We use a struct to simplify customization. Chassis::swingToHeading has many
 * parameters and specifying them all just to set one optional param harms
 * readability. By passing a struct to the function, we can have named
 * parameters, overcoming the c/c++ limitation
 *
 * @param forwards whether the robot should turn to face the point with the front of the robot.
 * True by default
 * @param direction the direction the robot should turn in. AUTO by default
 * @param maxSpeed the maximum speed the robot can turn at. Value between 0-127.
 *  127 by default
 * @param minSpeed the minimum speed the robot can turn at. If set to a non-zero value,
 *  the exit conditions will switch to less accurate but smoother ones. Value between 0-127.
 *  0 by default
 * @param earlyExitRange angle between the robot and target heading where the movement will
 *  exit. Only has an effect if minSpeed is non-zero.
 */
struct SwingToPointParams {
        bool forwards = true;
        AngularDirection direction = AngularDirection::AUTO;
        float maxSpeed = 127;
        float minSpeed = 0;
        float earlyExitRange = 0;
};

/**
 * @brief Parameters for Chassis::swingToHeading
 *
 * We use a struct to simplify customization. Chassis::swingToHeading has many
 * parameters and specifying them all just to set one optional param harms
 * readability. By passing a struct to the function, we can have named
 * parameters, overcoming the c/c++ limitation
 *
 * @param direction the direction the robot should turn in. AUTO by default
 * @param maxSpeed the maximum speed the robot can turn at. Value between 0-127.
 *  127 by default
 * @param minSpeed the minimum speed the robot can turn at. If set to a non-zero value,
 *  the exit conditions will switch to less accurate but smoother ones. Value between 0-127.
 *  0 by default
 * @param earlyExitRange angle between the robot and target heading where the movement will
 *  exit. Only has an effect if minSpeed is non-zero.
 */
struct SwingToHeadingParams {
        AngularDirection direction = AngularDirection::AUTO;
        float maxSpeed = 127;
        float minSpeed = 0;
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

// default drive curve
extern ExpoDriveCurve defaultDriveCurve;

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
         * @param throttleCurve curve applied to throttle input during driver control
         * @param turnCurve curve applied to steer input during driver control
         */
        Chassis(Drivetrain drivetrain, ControllerSettings linearSettings, ControllerSettings angularSettings,
                OdomSensors sensors, DriveCurve* throttleCurve = &defaultDriveCurve,
                DriveCurve* steerCurve = &defaultDriveCurve);
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
         * @param x x location
         * @param y y location
         * @param timeout longest time the robot can spend moving
         * @param params struct to simulate named parameters
         * @param async whether the function should be run asynchronously. true by default
         */
        void turnToPoint(float x, float y, int timeout, TurnToPointParams params = {}, bool async = true);
        /**
         * @brief Turn the chassis so it is facing the target heading
         *
         * @param theta heading location
         * @param timeout longest time the robot can spend moving
         * @param params struct to simulate named parameters
         * @param async whether the function should be run asynchronously. true by default
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
         * @brief Control the robot during the driver using the arcade drive control scheme. In this control scheme one
         * joystick axis controls the forwards and backwards movement of the robot, while the other joystick axis

         * controls  the robot's turning
         * @param throttle speed to move forward or backward. Takes an input from -127 to 127.
         * @param turn speed to turn. Takes an input from -127 to 127.
         * @param disableDriveCurve whether to disable the drive curve or not. If disabled, uses a linear curve with no
         * deadzone or minimum power
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
         * @param desaturateBias how much to favor turning over forward/backward motion or vice versa when motors are
         * saturated. A value of 0 fully prioritizes driving, a value of 1 fully prioritizes turning
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
         */
        void curvature(int throttle, int turn, bool disableDriveCurve = false);
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
        /**
         * PIDs are exposed so advanced users can implement things like gain scheduling
         * Changes are immediate and will affect a motion in progress
         *
         * @warning Do not interact with these unless you know what you are doing
         */
        PID lateralPID;
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
