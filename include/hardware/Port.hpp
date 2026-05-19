#pragma once

#include <cstdint>
#include <algorithm>

namespace lemlib {

namespace detail {

template <unsigned N> struct fixed_string {
        constexpr fixed_string(char const* s) { std::copy(s, s + N, buf); }

        constexpr operator char const*() const { return buf; }

        char buf[N + 1] {};
};

template <unsigned N> fixed_string(char const (&)[N]) -> fixed_string<N - 1>;

/**
 * @brief This function is intended to be used as an assertion in consteval functions.
 *
 * @warning It is UNDEFINED BEHAVIOR to call this function in a non-consteval context.
 *
 * @tparam Str The error message
 */
template <fixed_string Str> inline void ceval_assert() { __builtin_unreachable(); }

} // namespace detail

struct DynamicPort {};

constexpr DynamicPort runtime_check_port {};

class SmartPort {
    public:
        consteval SmartPort(std::int64_t port)
            : m_port(port) {
            if (port < 1 || port > 21) detail::ceval_assert<"Port out of range!">();
        }

        constexpr SmartPort(std::int64_t port, DynamicPort)
            : m_port(port) {
            if (port < 1 || port > 21) m_port = 0;
        }

        constexpr operator std::uint8_t() const { return m_port; }
    private:
        std::uint8_t m_port;
};

class ReversibleSmartPort {
    public:
        consteval ReversibleSmartPort(std::int64_t port)
            : m_port(port) {
            if (port < 0) port = -port;
            if (port < 1 || port > 21) detail::ceval_assert<"Port out of range!">();
        }

        constexpr ReversibleSmartPort(SmartPort port)
            : m_port(port) {}

        constexpr ReversibleSmartPort(std::int64_t port, DynamicPort)
            : m_port(port) {
            if (port < 0) port = -port;
            if (port < 1 || port > 21) m_port = 0;
        }

        constexpr ReversibleSmartPort operator-() const { return ReversibleSmartPort {-m_port, runtime_check_port}; }

        constexpr bool is_reversed() const { return m_port < 0; }

        [[nodiscard("This function does not mutate the original value")]] constexpr ReversibleSmartPort
        set_reversed(bool reversed) const {
            std::uint8_t port = m_port < 0 ? -m_port : m_port;
            return ReversibleSmartPort {reversed ? -port : port, runtime_check_port};
        }

        constexpr operator std::int8_t() const { return m_port; }
    private:
        std::int8_t m_port;
};

class ADIPort {
    public:
        consteval ADIPort(std::int64_t port)
            : m_port(0) {
            if (port >= 'a' && port <= 'h') port -= ('a' - 1);
            else if (port >= 'A' && port <= 'H') port -= ('A' - 1);
            if (port < 1 || port > 8) detail::ceval_assert<"Port out of range!">();
            m_port = port;
        }

        constexpr ADIPort(std::int64_t port, DynamicPort)
            : m_port(0) {
            if (port < 1 || port > 21) m_port = 0;
            if (port >= 'a' && port <= 'h') port -= ('a' - 1);
            else if (port >= 'A' && port <= 'H') port -= ('A' - 1);
            if (port < 1 || port > 8) return;
            m_port = port;
        }

        constexpr operator std::uint8_t() const { return m_port; }
    private:
        std::uint8_t m_port;
};

class ADIPair {
    public:
        consteval ADIPair(std::int64_t port_1, std::int64_t port_2)
            : m_port_1(port_1),
              m_port_2(port_2) {
            auto lower_port = std::min(std::uint8_t {m_port_1}, std::uint8_t {m_port_2});
            auto higher_port = std::max(std::uint8_t {m_port_1}, std::uint8_t {m_port_2});
            if (higher_port - lower_port != 1 || lower_port % 2 != 1)
                detail::ceval_assert<"Invalid pair: valid pairs are A&B, C&D, E&F, G&H">();
        }

        constexpr ADIPair(std::int64_t port_1, std::int64_t port_2, DynamicPort)
            : m_port_1(port_1, runtime_check_port),
              m_port_2(port_2, runtime_check_port) {
            auto lower_port = std::min(std::uint8_t {m_port_1}, std::uint8_t {m_port_2});
            auto higher_port = std::max(std::uint8_t {m_port_1}, std::uint8_t {m_port_2});
            if (higher_port - lower_port != 1 || lower_port % 2 != 1) {
                m_port_1 = ADIPort(0, runtime_check_port);
                m_port_2 = ADIPort(0, runtime_check_port);
            }
        }

        constexpr ADIPort first() const { return m_port_1; }

        constexpr ADIPort second() const { return m_port_2; }
    private:
        ADIPort m_port_1;
        ADIPort m_port_2;
};

} // namespace lemlib
