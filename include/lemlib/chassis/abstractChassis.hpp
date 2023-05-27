//
// Created by Rocky Chen on 2023/5/26.
//

#pragma once
#include "pros/motors.hpp"
#include "pros/imu.hpp"
#include "lemlib/chassis/trackingWheel.hpp"
#include "lemlib/pose.hpp"

namespace lemlib {

    class AbstractChassis {
    public:
    /**
     * @brief Struct containing all the sensors used for odometry
     *
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
        typedef struct {
            TrackingWheel* vertical1;
            TrackingWheel* vertical2;
            TrackingWheel* horizontal1;
            TrackingWheel* horizontal2;
            pros::Imu* imu;
        } OdomSensors_t;

        /**
         * @brief Struct containing constants for a chassis controller
         *
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
        typedef struct {
            float kP;
            float kD;
            float smallError;
            float smallErrorTimeout;
            float largeError;
            float largeErrorTimeout;
            float slew;
        } ChassisController_t;

        /**
         * @brief Struct containing constants for a drivetrain
         *
         */
        typedef struct {
        } Drivetrain_t;

    protected:
        AbstractChassis() = default;

        AbstractChassis(ChassisController_t lateralSettings, ChassisController_t angularSettings,
                        OdomSensors_t odomSensors);

        AbstractChassis(ChassisController_t lateralSettings, ChassisController_t angularSettings,
                        ChassisController_t strafeSettings, OdomSensors_t odomSensors);

        void operator=(const AbstractChassis& rhs) = delete;

       template<class> friend class ChassisBuilder;
    public:
        /**
         * @brief Calibrate the chassis sensors
         *
         */
        virtual void calibrate();
        /**
         * @brief Set the pose of the chassis
         *
         * @param x new x value
         * @param y new y value
         * @param theta new theta value
         * @param radians true if theta is in radians, false if not. False by default
         */
         void setPose(double x, double y, double theta, bool radians = false);
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
         Pose getPose(bool radians = false);

    protected:

        ChassisController_t lateralSettings;
        ChassisController_t angularSettings;
        ChassisController_t strafeSettings;
        Drivetrain_t drivetrain;
        OdomSensors_t odomSensors;
    };

} // lemlib

