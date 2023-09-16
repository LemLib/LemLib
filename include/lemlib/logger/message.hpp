#pragma once

#include <string>

namespace lemlib {
/**
 * @brief Level of the message
 *
 */
enum class Level { INFO, DEBUG, WARN, ERROR, FATAL };

/**
 * @brief A loggable message
 *
 */
struct Message {
        /* The (formatted) message */
        std::string message;

        /** The level of the message */
        Level level;
};

/**
 * @brief Format a level
 *
 * @param level
 * @return std::string
 */
std::string format_as(Level level);
} // namespace lemlib