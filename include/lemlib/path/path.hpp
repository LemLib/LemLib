/**
 * @file include/lemlib/path.hpp
 * @author LemLib Team
 * @brief
 * @version 0.6.0
 * @date 2023-11-20
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once

#include "lemlib/asset.hpp"
#include "lemlib/pose.hpp"
#include <string>
#include <vector>

namespace lemlib {
class Path {
    public:
        /**
         * @brief Construct a new Path
         *
         */
        Path() {};

        /**
         * @brief Get the path data.
         *
         * @return std::vector<lemlib::Pose> the path data, as a vector of poses
         */
        std::vector<lemlib::Waypoint> getPath();
    protected:
        /**
         * @brief Reads the data from the asset or path and stores it in the data vector.
         */
        void load(std::vector<std::string> lines);
        std::vector<lemlib::Waypoint> path;
};
} // namespace lemlib