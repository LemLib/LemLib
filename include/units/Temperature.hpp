#pragma once

#include "units/units.hpp"

using Temperature = Quantity<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<1>,
                             std::ratio<0>, std::ratio<0>>;

constexpr Temperature kelvin = Temperature(1.0);

constexpr Temperature operator""_kelvin(long double value) { return Temperature(static_cast<double>(value)); }

constexpr Temperature operator""_kelvin(unsigned long long value) { return Temperature(static_cast<double>(value)); }

constexpr Temperature operator""_celsius(long double value) { return Temperature(static_cast<double>(value) + 273.15); }

constexpr Temperature operator""_celsius(unsigned long long value) {
    return Temperature(static_cast<double>(value) + 273.15);
}

constexpr Temperature operator""_fahrenheit(long double value) {
    return Temperature((static_cast<double>(value) - 32) * (5.0 / 9.0) + 273.5);
}

constexpr Temperature operator""_fahrenheit(unsigned long long value) {
    return Temperature((static_cast<double>(value) - 32) * (5.0 / 9.0) + 273.5);
}

namespace units {

constexpr inline Temperature from_kelvin(double value) { return Temperature(value); }

constexpr inline double to_kelvin(Temperature quantity) { return quantity.val(); }

constexpr inline Temperature from_celsius(double value) { return Temperature(value + 273.15); }

constexpr inline double to_celsius(Temperature quantity) { return quantity.val() - 273.15; }

constexpr inline Temperature from_fahrenheit(double value) { return Temperature((value - 32) * (5.0 / 9.0) + 273.15); }

constexpr inline double to_fahrenheit(Temperature quantity) { return (quantity.val() - 273.15) * (9.0 / 5.0) + 32; }

} // namespace units