/**
 * @file include/lemlib/chassis/path.hpp
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
#include <vector>

namespace lemlib {
class Path {
    public:
        /**
         * @brief Creates a new Path object using a static asset
         * 
         * @param path the asset to be read
         */
        Path(const asset& path);
        /**
         * @brief Creates a new Path object from a file on the SD card
         * 
         * @param path the path to the file on the SD card. Don't include the "/usd/" prefix
         */
        Path(const char* path);

        /**
         * @brief Get the path data.
         * 
         * @return std::vector<lemlib::Pose> the path data, as a vector of poses 
         */
        std::vector<lemlib::Pose> getData();
    protected:
        /**
         * @brief Loads the data from the asset or path
         * Is called by the constructor.
         */
        virtual void load() = 0;
    private:
        std::vector<lemlib::Pose> data;
};
}