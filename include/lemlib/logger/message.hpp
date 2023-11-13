#pragma once

#include <string>

namespace lemlib {
/**
 * @brief Level of the message
 *
 */
enum class Level { TELEMETRY, INFO, DEBUG, WARN, ERROR, FATAL };

/**
 * @brief A loggable message
 *
 */
struct Message {
        /* The message */
        std::string message;

        /** The level of the message */
        Level level;

        /** The time the message was logged, in milliseconds */
        uint32_t time;
};

/**
 * @brief Format a level
 *
 * @param level The level to format
 * @return std::string The formatted level, in caps (ex. info -> INFO)
 */
std::string format_as(Level level);
} // namespace lemlib
