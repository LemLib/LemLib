#pragma once

#include "pros/rtos.hpp" // IWYU pragma: keep

#define WAIT_UNTIL(c)                                                                                                  \
    while (!(c)) pros::delay(5);
