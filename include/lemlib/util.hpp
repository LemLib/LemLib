#pragma once

namespace lemlib {
template <typename T> constexpr T sgn(T val) { return (T(0) < val) - (val < T(0)); }
} // namespace lemlib