#pragma once

namespace lemlib {
/**
 * @brief Enum class DriveSide
 *
 * When using swing turns, the user needs to specify what side of the drivetrain should be locked
 * we could just use an integer or boolean for this, but using an enum class improves readability
 *
 * This enum class only has 2 values, LEFT and RIGHT
 */
enum class DriveSide {
    LEFT, /** lock the left side of the drivetrain */
    RIGHT /** lock the right side of the drivetrain */
};

/**
 * @brief AngularDirection
 *
 * When turning, the user may want to specify the direction the robot should turn in.
 * This enum class has 3 values: CW_CLOCKWISE, CCW_COUNTERCLOCKWISE, and AUTO
 * AUTO will make the robot turn in the shortest direction, and will be the most used value
 */
enum class AngularDirection {
    CW_CLOCKWISE, /** turn clockwise */
    CCW_COUNTERCLOCKWISE, /** turn counter-clockwise */
    AUTO /** turn in the direction with the shortest distance to target */
};
} // namespace lemlib
