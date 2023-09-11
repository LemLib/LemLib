#include "abstractLogger.hpp"

void lemlib::AbstractLogger::setLowestLevel(lemlib::AbstractLogger::Level lowestLevel) {
    this->lowestLevel = lowestLevel;
}