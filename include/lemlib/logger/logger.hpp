#pragma once

#include "baseSink.hpp"

#include "infoSink.hpp"
#include "telemetrySink.hpp"

#define FMT_HEADER_ONLY
#include "fmt/core.h"

#include <memory>
#include <array>

namespace lemlib {
class Logger {
    public:
        /**
         * @brief Get the info sink
         *
         * @return InfoSink
         */
        static InfoSink& getInfoSink();

        /**
         * @brief Get the telemetry sink
         *
         * @return TelemetrySink&
         */
        static TelemetrySink& getTelemetrySink();

        /**
         * @brief Log to the info logger
         *
         * @tparam T
         * @param level
         * @param format
         * @param args
         */
        template <typename... T> static void log(Level level, fmt::format_string<T...> format, T&&... args) {
            getInfoSink().log(level, format, std::forward<T>(args)...);
        }

        /**
         * @brief Log a debug message to the info logger
         *
         * @tparam T
         * @param format
         * @param args
         */
        template <typename... T> static void debug(fmt::format_string<T...> format, T&&... args) {
            log(Level::DEBUG, format, std::forward<T>(args)...);
        }

        /**
         * @brief Log a info message to the info logger
         *
         * @tparam T
         * @param format
         * @param args
         */
        template <typename... T> static void info(fmt::format_string<T...> format, T&&... args) {
            log(Level::INFO, format, std::forward<T>(args)...);
        }

        /**
         * @brief Log a warn message to the info logger
         *
         * @tparam T
         * @param format
         * @param args
         */
        template <typename... T> static void warn(fmt::format_string<T...> format, T&&... args) {
            log(Level::WARN, format, std::forward<T>(args)...);
        }

        /**
         * @brief Log a error message to the info logger
         *
         * @tparam T
         * @param format
         * @param args
         */
        template <typename... T> static void error(fmt::format_string<T...> format, T&&... args) {
            log(Level::ERROR, format, std::forward<T>(args)...);
        }

        /**
         * @brief Log a fatal message to the info logger
         *
         * @tparam T
         * @param format
         * @param args
         */
        template <typename... T> static void fatal(fmt::format_string<T...> format, T&&... args) {
            log(Level::FATAL, format, std::forward<T>(args)...);
        }

        /**
         * @brief Log to the telemetry logger
         *
         * @tparam T
         * @param level
         * @param format
         * @param args
         */
        template <typename... T> static void teleLog(Level level, fmt::format_string<T...> format, T&&... args) {
            getTelemetrySink().log(level, format, std::forward<T>(args)...);
        }

        /**
         * @brief Set the id of the telemetry messages
         *
         * @param id
         */
        static void setTeleId(uint8_t id) { getTelemetrySink().setTelemetryId(id); }

        /**
         * @brief Log a debug message to the telemetry logger
         *
         * @tparam T
         * @param format
         * @param args
         */
        template <typename... T> static void teleDebug(fmt::format_string<T...> format, T&&... args) {
            teleLog(Level::DEBUG, format, std::forward<T>(args)...);
        }

        /**
         * @brief Log a info message to the telemetry logger
         *
         * @tparam T
         * @param format
         * @param args
         */
        template <typename... T> static void teleInfo(fmt::format_string<T...> format, T&&... args) {
            teleLog(Level::INFO, format, std::forward<T>(args)...);
        }

        /**
         * @brief Log a warn message to the telemetry logger
         *
         * @tparam T
         * @param format
         * @param args
         */
        template <typename... T> static void teleWarn(fmt::format_string<T...> format, T&&... args) {
            teleLog(Level::WARN, format, std::forward<T>(args)...);
        }

        /**
         * @brief Log a error message to the telemetry logger
         *
         * @tparam T
         * @param format
         * @param args
         */
        template <typename... T> static void teleError(fmt::format_string<T...> format, T&&... args) {
            teleLog(Level::ERROR, format, std::forward<T>(args)...);
        }

        /**
         * @brief Log a fatal message to the telemetry logger
         *
         * @tparam T
         * @param format
         * @param args
         */
        template <typename... T> static void teleFatal(fmt::format_string<T...> format, T&&... args) {
            teleLog(Level::FATAL, format, std::forward<T>(args)...);
        }
};
} // namespace lemlib