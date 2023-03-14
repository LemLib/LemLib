/**
 * @file lemlib/chassis/odom.hpp
 * @author Liam Teale (you@domain.com)
 * @brief This is the header file for the odom.cpp file. Its not meant to be used directly, only through the chassis class
 * @version 0.1
 * @date 2023-01-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once

#include "lemlib/chassis/chassis.hpp"
#include "lemlib/pose.hpp"


namespace lemlib {
    /**
     * @brief Set the sensors to be used for odometry
     * 
     * @param sensors the sensors to be used
     * @param drivetrain drivetrain to be used
     */
    void setSensors(lemlib::OdomSensors_t sensors, lemlib::Drivetrain_t drivetrain);
    /**
     * @brief Get the pose of the robot
     * 
     * @param radians true for theta in radians, false for degrees. False by default
     * @return Pose 
     */
    Pose getPose(bool radians = false);
    /**
     * @brief Set the Pose of the robot
     * 
     * @param pose the new pose
     * @param radians true if theta is in radians, false if in degrees. False by default
     */
    void setPose(Pose pose, bool radians = false);
    /**
     * @brief Update the pose of the robot
     * 
     */
    void update();
    /**
     * @brief Initialize the odometry system
     * 
     */
    void init();
}
