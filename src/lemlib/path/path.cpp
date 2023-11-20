#include "lemlib/path/path.hpp"
#include "lemlib/util.hpp"

std::vector<lemlib::Waypoint> lemlib::Path::getPath() { return this->path; }

void lemlib::Path::load(std::vector<std::string> lines) {
    for (const std::string& line : lines) { // loop through all lines
        if (line == "endData" || line == "endData\r") break;
        std::vector<std::string> pointInput = splitString(line, ", "); // parse line
        const float x = std::stof(pointInput.at(0)); // x position
        const float y = std::stof(pointInput.at(1)); // y position
        const float speed = std::stof(pointInput.at(2)); // speed
        this->path.push_back({x, y, 0, speed}); // save data
    }
}