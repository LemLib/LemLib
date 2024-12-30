#include "lemlib/util.hpp"

double lemlib::sgn(double val) { return (0 < val) - (val < 0); }