#pragma once

#include <cstdint>

namespace lemlib {
/**
 * @brief abstract Device class
 *
 * used to have a generic interface for devices
 */
class Device {
    public:
        virtual int32_t isConnected() const = 0;
};
} // namespace lemlib
