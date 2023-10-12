#include "lemlib/odom/arc.hpp"

lemlib::ArcOdom::ArcOdom(std::vector<TrackingWheel>& verticals, std::vector<TrackingWheel>& horizontals,
                         std::vector<pros::Imu>& imus)
    : verticals(verticals),
      horizontals(horizontals),
      imus(imus) {}