#pragma once

#include <climits>

namespace lemlib {
/**
 * @brief Convert a PROS function return to a standard return
 *
 * The C/C++ standard specifies that function should return 0 on success, and anything other than 0 is to be considered
 * an error. PROS uses a variation of this, where functions return 1 on success.
 *
 * If the status passed to this function equals 1, it will return 0 to indicate a success. Otherwise, it will return
 * INT_MAX to indicate an error
 *
 * @return 0 on success
 * @return INT_MAX on failure
 *
 * @b Example:
 * @code {.cpp}
 * int setReversed(int port, bool reversed) {
 *     return convertStatus(pros::c::motor_set_reversed(port, reversed));
 * }
 * @endcode
 */
inline int convertStatus(int status) {
    if (status == 1) return 0;
    else return INT_MAX;
}
} // namespace lemlib