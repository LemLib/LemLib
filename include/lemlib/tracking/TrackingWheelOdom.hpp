#pragma once

#include "hardware/encoder/Encoder.hpp"
#include "hardware/IMU/Imu.hpp"
#include "pros/rtos.hpp"
#include "units/Pose.hpp"
#include <vector>

namespace lemlib {
/**
 * @brief class representing a tracking wheel
 *
 */
class TrackingWheel {
    public:
        /**
         * @brief Construct a new Tracking Wheel object
         *
         * @param encoder pointer to the encoder which should be used for tracking
         * @param diameter the diameter of the wheel
         * @param offset how far the tracking wheel is from the turning center
         *
         * @b Example:
         * @code {.cpp}
         * // create rotation senor on port 3, which is reversed
         * lemlib::V5RotationSensor encoder(3, true);
         * // create a tracking wheel using the encoder above, which has a wheel diameter
         * // of 2.75 inches and is offset 3 inches to the left
         * lemlib::TrackingWheel trackingWheel(&encoder, 2.75_in, -3_in);
         * @endcode
         */
        TrackingWheel(Encoder* encoder, Length diameter, Length offset);
        /**
         * @brief Get the distance traveled by the tracking wheel since this function was last called.
         * This function is not thread safe.
         *
         * It is recommended to set the angle of the encoder to 0 before starting to use this function.
         *
         * Since the internal encoder object is abstract, it's not known what values errno may be
         * set to in case of a failure.
         *
         * @return INFINITY an error has occurred, possibly setting errno
         * @return Length the distance the tracking wheel has traveled since the last time
         * the function was called
         *
         * @b Example:
         * @code {.cpp}
         * void autonomous() {
         *   // create tracking wheel
         *   lemlib::V5RotationSensor encoder(3, true);
         *   lemlib::TrackingWheel trackingWheel(&encoder, 2.75_in, -3_in);
         *   // prevent weird values on the first cycle by setting the encoder
         *   // angle to 0
         *   trackingWheel.encoder->setAngle(0_stDeg);
         *
         *   // loop forever
         *   while (true) {
         *     std::cout << "distance traveled since last cycle: "
         *               << trackingWheel.getDistanceTraveled()
         *               << std::endl;
         *     pros::delay(10);
         *   }
         * }
         * @endcode
         */
        Length getDistanceDelta();
        /**
         * @brief Get the distance traveled by the tracking wheel
         *
         * Since the internal encoder object is abstract, it's not known what values errno may be
         * set to in case of a failure.
         *
         * @return INFINITY an error has occurred, possibly setting errno
         * @return Length the distance the tracking wheel has traveled since the last time
         * the function was called
         *
         * @b Example:
         * @code {.cpp}
         * void autonomous() {
         *   // create tracking wheel
         *   lemlib::V5RotationSensor encoder(3, true);
         *   lemlib::TrackingWheel trackingWheel(&encoder, 2.75_in, -3_in);
         *
         *   // loop forever
         *   while (true) {
         *     std::cout << "distance traveled: "
         *               << trackingWheel.getDistanceTraveled()
         *               << std::endl;
         *     pros::delay(10);
         *   }
         * }
         * @endcode
         */
        Length getDistanceTraveled();
        /**
         * @brief Get the offset of the tracking wheel
         *
         * @return Length the offset of the tracking wheel
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *   lemlib::V5RotationSensor encoder(3, true);
         *   lemlib::TrackingWheel trackingWheel(&encoder, 2.75_in, -3_in);
         *   std::cout << to_in(trackingWheel.getOffset()) << std::endl; // outputs -3
         * }
         * @endcode
         */
        Length getOffset();
        /**
         * @brief reset the tracking wheel encoder
         *
         * Since the internal encoder object is abstract, it's not known what values errno may be
         * set to in case of a failure.
         *
         * @return INT_MAX an error has occurred, possibly setting errno
         *
         * @b Example:
         * @code {.cpp}
         * void initialize() {
         *   lemlib::V5RotationSensor encoder(3, true);
         *   encoder.setAngle(15_stDeg);
         *   lemlib::TrackingWheel trackingWheel(&encoder, 2.75_in, -3_in);
         *   trackingWheel.reset();
         *   std::cout << to_in(encoder.getDistanceTraveled()) << std::endl; // outputs 0
         * }
         * @endcode
         */
        int reset();
    private:
        Encoder* m_encoder;
        Length m_diameter;
        Length m_offset;
        Length m_lastTotal;
};

/**
 * @brief Tracking Wheel Odometry class
 *
 * This class can be used to track the position a differential drive robot,
 * which has any number of tracking wheels and IMUs, including 0
 */
class TrackingWheelOdometry {
    public:
        /**
         * @brief Construct a new Tracking Wheel Odometry object
         *
         * @param imus vector of Imu pointers
         * @param verticalWheels vector of vertical tracking wheels
         * @param horizontalWheels vector of horizontal tracking wheels
         *
         * @b Example:
         * @code {.cpp}
         * // In this example, the TrackingWheelOdometry class will use the average
         * // value of 2 imus to find the heading of the robot
         * // a single vertical tracking wheel will be used to track the local y
         * // displacement of the robot
         * // the local x displacement of the robot will be assumed to be 0, as
         * // there are no sensors available for this measurement
         *
         * // create IMUs
         * lemlib::V5InertialSensor imu1(1);
         * lemlib::V5InertialSensor imu2(2);
         *
         * // create a vertical tracking wheel
         * lemlib::V5RotationSensor verticalEncoder(3, false);
         * lemlib::TrackingWheel verticalWheel(&verticalEncoder, 2.75_in, 2_in);
         *
         * // no horizontal tracking wheels for this example
         *
         * // create a TrackingWheelOdometry instance
         * lemlib::TrackingWheelOdometry odom({imu1, imu2}, {verticalWheel}, {});
         * @endcode
         *
         * @b Example:
         * @code {.cpp}
         * // in this example, the TrackingWheelOdometry class will calculate
         * // the heading using 2 horizontal tracking wheels
         * // a single vertical tracking wheel will be used to calculate the local
         * // y displacement of the robot
         * // a single horizontal tracking wheel will be used to calculate the local
         * // x displacement of the robot
         *
         * // no IMUs for this example
         *
         * // create a vertical tracking wheel
         * lemlib::V5RotationSensor verticalEncoder(1, false);
         * lemlib::TrackingWheel verticalWheel(&verticalEncoder, 2.75_in, 2_in);
         *
         * // create 2 horizontal tracking wheels
         * lemlib::ADIEncoder horizontalEncoder1('a', 'b', true);
         * lemlib::ADIEncoder2 horizontalEncoder2(2, 'c', 'd', false);
         * lemlib::TrackingWheel horizontalWheel1(&horizontalEncoder1, 3.25_in, -2_in);
         * lemlib::TrackingWheel horizontalWheel2(&horizontalEncoder2, 2.75_in, -4_in);
         *
         * // create a TrackingWheelOdometry instance
         * lemlib::TrackingWheelOdometry odom({}, {verticalWheel}, {horizontalWheel1, horizontalWheel2});
         * @endcode
         */
        TrackingWheelOdometry(std::vector<IMU*> imus, std::vector<TrackingWheel> verticalWheels,
                              std::vector<TrackingWheel> horizontalWheels);
        /**
         * @brief Get the estimated Pose of the robot
         *
         * @return units::Pose the estimated pose
         *
         * @b Example:
         * // create TrackingWheelOdom object
         * lemlib::TrackingWheelOdom odom(...);
         *
         * @code {.cpp}
         * void initialize() {
         *   pros::lcd::initialize(); // initialize brain screen
         *   odom.calibrate(); // calibrate odom
         *   while (true) {
         *     // get current pose of the robot
         *     const lemlib::Pose pose = odom.getPose();
         *     // print pros to the brain screen
         *     pros::lcd::print(0, "x: %f", to_in(pose.x));
         *     pros::lcd::print(1, "y: %f", to_in(pose.y));
         *     pros::lcd::print(2, "theta: %f", to_in(pose.theta));
         *     // delay to let other tasks run
         *     pros::delay(10);
         *   }
         * }
         * @endcode
         */
        units::Pose getPose();
        /**
         * @brief Set the estimated pose of the robot
         *
         * @param pose the new pose
         *
         * @b Example:
         * @code {.cpp}
         * // create TrackingWheelOdom object
         * lemlib::TrackingWheelOdom odom(...);
         *
         * void autonomous() {
         *   // set the starting position of the robot
         *   odom.setPose({15_in, -12_in, 90_cDeg});
         * }
         * @endcode
         */
        void setPose(units::Pose pose);
        /**
         * @brief start the tracking task. Sensors need to be calibrated beforehand
         *
         * Starts the tracking task if it has not been started yet.
         * Nothing happens if the task has already been started.
         * Sensors need to be calibrated before this function is called.
         *
         * @param period how long to wait before updating the task again. Defaults to 10 ms
         *
         * @b Example:
         * @code {.cpp}
         * // create TrackingWheelOdom object
         * lemlib::TrackingWheelOdom odom(...);
         *
         * void initialize() {
         *   // calibrate sensors
         *   imu.calibrate();
         *   // start the tracking task
         *   odom.startTask();
         *   // now we can get position data
         * }
         * @endcode
         *
         * @b Example:
         * @code {.cpp}
         * // create TrackingWheelOdom object
         * lemlib::TrackingWheelOdom odom(...);
         *
         * void initialize() {
         *   // calibrate sensors
         *   imu.calibrate();
         *   // start the tracking task
         *   // and have it update every 20 milliseconds
         *   // or, in other words, 50 times per second
         *   odom.startTask(20_msec);
         *   // now we can get position data
         * }
         * @endcode
         */
        void startTask(Time period = 10_msec);
        /**
         * @brief Destroy the Tracking Wheel Odometry object. Stops the tracking task
         *
         * De-allocation of IMU pointers is up to the caller.
         */
        ~TrackingWheelOdometry();
    private:
        /**
         * @brief update the estimated pose
         *
         * This function should have its own dedicated task
         */
        void update(Time period);
        units::Pose m_pose = {0_m, 0_m, 0_stDeg};
        Angle m_offset = 0_stDeg;
        std::optional<pros::Task> m_task = std::nullopt;
        std::vector<IMU*> m_Imus;
        std::vector<TrackingWheel> m_verticalWheels;
        std::vector<TrackingWheel> m_horizontalWheels;
};
} // namespace lemlib
