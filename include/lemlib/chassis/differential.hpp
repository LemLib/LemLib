/**
 * @file include/lemlib/chassis/chassis.hpp
 * @author LemLib Team
 * @brief Chassis class declarations
 * @version 0.4.5
 * @date 2023-01-23
 *
 * @copyright Copyright (c) 2023
 *
 */

#pragma once

#include <functional>
#include <memory>

#include "pros/rtos.hpp"
#include "pros/motors.hpp"

#include "lemlib/chassis/chassis.hpp"
#include "lemlib/asset.hpp"
#include "lemlib/devices/trackingWheel.hpp"
#include "lemlib/devices/gyro/imu.hpp"

namespace lemlib {
/**
 * @brief Construct a shared pointer to a tracking wheel.
 *
 * This function exists to reduce complexity for the client. The client could make their own
 * shared pointer to a motor group, but this function makes it easy
 *
 * @param ports array of signed ports. Negative ports mean the motor should be reversed
 * @param gears the gearbox used by the motors
 * @return std::shared_ptr<pros::MotorGroup> a shared pointer to the motor group
 */
[[nodiscard("makeMotorGroup function returns a shared_ptr of pros::MotorGroup, you can use "
            "std::shared_ptr<pros::MotorGroup> or auto to get the returned pointer")]] std::shared_ptr<pros::MotorGroup>
makeMotorGroup(const std::initializer_list<int8_t>& ports, const pros::v5::MotorGears& gears);

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
                    TrackingWheel* horizontal2, pros::Imu* imu)
            : vertical1(vertical1),
              vertical2(vertical2),
              horizontal1(horizontal1),
              horizontal2(horizontal2),
              gyro(std::make_shared<lemlib::Imu>(*imu)) {}

        /**
         * The sensors are stored in a struct so that they can be easily passed to the chassis class
         * The variables are pointers so that they can be set to nullptr if they are not used
         * Otherwise the chassis class would have to have a constructor for each possible combination of sensors
         *
         * @param vertical1 pointer to the first vertical tracking wheel
         * @param vertical2 pointer to the second vertical tracking wheel
         * @param horizontal1 pointer to the first horizontal tracking wheel
         * @param horizontal2 pointer to the second horizontal tracking wheel
         * @param gyro shared pointer to a gyro
         */
        OdomSensors(TrackingWheel* vertical1, TrackingWheel* vertical2, TrackingWheel* horizontal1,
                    TrackingWheel* horizontal2, std::shared_ptr<Gyro> gyro)
            : vertical1(vertical1),
              vertical2(vertical2),
              horizontal1(horizontal1),
              horizontal2(horizontal2),
              gyro(gyro) {}

        TrackingWheel* vertical1;
        TrackingWheel* vertical2;
        TrackingWheel* horizontal1;
        TrackingWheel* horizontal2;
        std::shared_ptr<Gyro> gyro;
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
         * @param kD derivative constant for the chassis controller
         * @param smallError the error at which the chassis controller will switch to a slower control loop
         * @param smallErrorTimeout the time the chassis controller will wait before switching to a slower control loop
         * @param largeError the error at which the chassis controller will switch to a faster control loop
         * @param largeErrorTimeout the time the chassis controller will wait before switching to a faster control loop
         * @param slew the maximum acceleration of the chassis controller
         */
        ControllerSettings(float kP, float kD, float smallError, float smallErrorTimeout, float largeError,
                           float largeErrorTimeout, float slew)
            : kP(kP),
              kD(kD),
              smallError(smallError),
              smallErrorTimeout(smallErrorTimeout),
              largeError(largeError),
              largeErrorTimeout(largeErrorTimeout),
              slew(slew) {}

        float kP;
        float kD;
        float smallError;
        float smallErrorTimeout;
        float largeError;
        float largeErrorTimeout;
        float slew;
};

/**
 * @brief Struct containing constants for a drivetrain
 *
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
struct Drivetrain {
        /**
         * The constants are stored in a struct so that they can be easily passed to the chassis class
         * Set a constant to 0 and it will be ignored
         *
         * @param leftMotors shared pointer to the left motors
         * @param rightMotors shared pointer to the right motors
         * @param trackWidth the track width of the robot
         * @param wheelDiameter the diameter of the wheel used on the drivetrain
         * @param rpm the rpm of the wheels
         * @param chasePower higher values make the robot move faster but causes more overshoot on turns
         */
        Drivetrain(const std::shared_ptr<pros::MotorGroup>& leftMotors,
                   const std::shared_ptr<pros::MotorGroup>& rightMotors, float trackWidth, float wheelDiameter,
                   float rpm, float chasePower)
            : leftMotors(leftMotors),
              rightMotors(rightMotors),
              trackWidth(trackWidth),
              wheelDiameter(wheelDiameter),
              rpm(rpm),
              chasePower(chasePower) {}

        std::shared_ptr<pros::MotorGroup> leftMotors;
        std::shared_ptr<pros::MotorGroup> rightMotors;
        float trackWidth;
        float wheelDiameter;
        float rpm;
        float chasePower;
};

/**
 * @brief Function pointer type for drive curve functions.
 *
 * @param input The control input in the range [-127, 127].
 * @param scale The scaling factor, which can be optionally ignored.
 * @return The new value to be used.
 */
typedef std::function<float(float, float)> DriveCurveFunction_t;

/**
 * @brief  Default drive curve. Modifies  the input with an exponential curve. If the input is 127, the function
 * will always output 127, no matter the value of scale, likewise for -127. This curve was inspired by team 5225, the
 * Pilons. A Desmos graph of this curve can be found here: https://www.desmos.com/calculator/rcfjjg83zx
 *
 * @param input value from -127 to 127
 * @param scale how steep the curve should be.
 * @return The new value to be used.
 */
float defaultDriveCurve(float input, float scale);

/**
 * @brief Chassis class
 *
 */
class Differential : public Chassis {
    public:
        /**
         * @brief Construct a new Chassis
         *
         * @param drivetrain drivetrain to be used for the chassis
         * @param linearSettings settings for the linear controller
         * @param angularSettings settings for the angular controller
         * @param sensors sensors to be used for odometry
         */
        Differential(const Drivetrain& drivetrain, const ControllerSettings& linearSettings,
                     const ControllerSettings& angularSettings, const OdomSensors& sensors);

        /**
         * @brief Initialize the chassis
         *
         */
        void initialize() override;

        /**
         * @brief Turn the chassis so it is facing a point
         *
         * The PID logging id is "angularPID"
         *
         * @param x x location
         * @param y y location
         * @param timeout longest time the robot can spend moving
         * @param reversed whether the robot should turn to face the point with the back of the robot. false by default
         * @param maxSpeed the maximum speed the robot can turn at. Default is 127
         */
        void turnToPose(float x, float y, int timeout, bool reversed = false, int maxSpeed = 127);

        /**
         * @brief Turn the chassis so it is facing a heading
         *
         * The PID logging id is "angularPID"
         *
         * @param heading the heading the robot should face. Units are in degrees
         * @param timeout longest time the robot can spend moving
         * @param maxSpeed the maximum speed the robot can turn at. Default is 127
         */
        void turnToHeading(float heading, int timeout, int maxSpeed = 127);

        /**
         * @brief Move the chassis towards the target pose
         *
         * Uses the boomerang controller
         *
         * @param x x location
         * @param y y location
         * @param theta theta (in degrees). Target angle
         * @param timeout longest time the robot can spend moving
         * @param reversed whether the robot should move forwards or backwards. true for backwards, false for
         * forwards (default)
         * @param lead the lead parameter. Determines how curved the robot will move. 0.6 by default (0 < lead < 1)
         * @param chasePower higher values make the robot move faster but causes more overshoot on turns. 0 makes it
         * default to global value
         * @param maxSpeed the maximum speed the robot can move at. 127 at default
         */
        void moveTo(float x, float y, float theta, int timeout, bool reversed = false, float chasePower = 0,
                    float lead = 0.6, int maxSpeed = 127);

        /**
         * @brief Move the chassis along a path
         *
         * @param filePath the filename of the path to follow
         * @param lookahead the lookahead distance. Units in inches. Larger values will make the robot move faster but
         * will follow the path less accurately
         * @param timeout the maximum time the robot can spend moving
         * @param reversed whether the robot should follow the path in reverse. false by default
         * @param maxSpeed the maximum speed the robot can move at
         */
        void follow(const asset& path, float lookahead, int timeout, bool reversed = false, int maxSpeed = 127);

        /**
         * @brief Control the robot during the driver control period using the tank drive control scheme. In this
         * control scheme one joystick axis controls one half of the robot, and another joystick axis controls another.
         * @param left speed of the left side of the drivetrain. Takes an input from -127 to 127.
         * @param right speed of the right side of the drivetrain. Takes an input from -127 to 127.
         * @param curveGain control how steep the drive curve is. The larger the number, the steeper the curve. A value
         * of 0 disables the curve entirely.
         */
        void tank(int left, int right, float leftCurveGain = 0.0, float rightCurveGain = 0.0,
                  const DriveCurveFunction_t& leftCurve = defaultDriveCurve,
                  const DriveCurveFunction_t& rightCurve = defaultDriveCurve);

        /**
         * @brief Control the robot during the driver using the arcade drive control scheme. In this control scheme one
         * joystick axis controls the forwards and backwards movement of the robot, while the other joystick axis
         * controls  the robot's turning
         * @param throttle speed to move forward or backward. Takes an input from -127 to 127.
         * @param turn speed to turn. Takes an input from -127 to 127.
         * @param curveGain the scale inputted into the drive curve function. If you are using the default drive
         * curve, refer to the `defaultDriveCurve` documentation.
         */
        void arcade(int throttle, int turn, float linearCurveGain = 0.0, float turnCurveGain = 0.0,
                    const DriveCurveFunction_t& leftCurve = defaultDriveCurve,
                    const DriveCurveFunction_t& rightCurve = defaultDriveCurve);
        /**
         * @brief Control the robot during the driver using the curvature drive control scheme. This control scheme is
         * very similar to arcade drive, except the second joystick axis controls the radius of the curve that the
         * drivetrain makes, rather than the speed. This means that the driver can accelerate in a turn without changing
         * the radius of that turn. This control scheme defaults to arcade when forward is zero.
         * @param throttle speed to move forward or backward. Takes an input from -127 to 127.
         * @param turn speed to turn. Takes an input from -127 to 127.
         * @param curveGain the scale inputted into the drive curve function. If you are using the default drive
         * curve, refer to the `defaultDriveCurve` documentation.
         */
        void curvature(int throttle, int turn, float linearCurveGain = 0.0, float turnCurveGain = 0.0,
                       const DriveCurveFunction_t& driveCurve = defaultDriveCurve,
                       const DriveCurveFunction_t& turnCurve = defaultDriveCurve);
    private:
        /**
         * @brief Chassis update function. Updates chassis motion and odometry
         *
         */
        void update() override;

        std::unique_ptr<ControllerSettings> linearSettings;
        std::unique_ptr<ControllerSettings> angularSettings;
        std::unique_ptr<Drivetrain> drivetrain;
};
} // namespace lemlib
