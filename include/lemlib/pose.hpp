/**
 * @file lemlib/pose.hpp
 * @author Liam Teale
 * @brief Pose class declarations
 * @version 0.1
 * @date 2023-01-23
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once


namespace lemlib {
    class Pose {
        public:
            float x;
            float y;
            float theta;
            /**
             * @brief Create a new pose
             * 
             * @param x component
             * @param y component
             * @param theta heading. Defaults to 0
             */
            Pose(float x, float y, float theta = 0);
            /**
             * @brief Add a pose to this pose
             * 
             * @param other other pose
             * @return Pose 
             */
            Pose operator + (const Pose &other);
            /**
             * @brief Subtract a pose from this pose
             * 
             * @param other other pose
             * @return Pose 
             */
            Pose operator - (const Pose &other);
            /**
             * @brief Multiply a pose by this pose
             * 
             * @param other other pose
             * @return Pose 
             */
            float operator * (const Pose &other);
            /**
             * @brief Multiply a pose by a float
             * 
             * @param other float
             * @return Pose 
             */
            Pose operator * (const float &other);
            /**
             * @brief Divide a pose by a float
             * 
             * @param other float
             * @return Pose 
             */
            Pose operator / (const float &other);
            /**
             * @brief Linearly interpolate between two poses
             * 
             * @param other the other pose
             * @param t t value
             * @return Pose 
             */
            Pose lerp(Pose other, float t);
            /**
             * @brief Get the distance between two poses
             * 
             * @param other the other pose
             * @return float 
             */
            float distance(Pose other);
            /**
             * @brief Get the angle between two poses
             * 
             * @param other the other pose
             * @return float in radians
             */
            float angle(Pose other);
            /**
             * @brief Rotate a pose by an angle
             * 
             * @param angle angle in radians
             * @return Pose 
             */
            Pose rotate(float angle);
    };
}
