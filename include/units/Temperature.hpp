#pragma once

#include "units/units.hpp"

class Temperature : public Quantity<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>,
                                    std::ratio<1>, std::ratio<0>, std::ratio<0>> {
    public:
        explicit constexpr Temperature(double value)
            : Quantity<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<1>,
                       std::ratio<0>, std::ratio<0>>(value) {}

        constexpr Temperature(Quantity<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>,
                                       std::ratio<1>, std::ratio<0>, std::ratio<0>>
                                  value)
            : Quantity<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<1>,
                       std::ratio<0>, std::ratio<0>>(value) {};
};

template <> struct LookupName<Quantity<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>,
                                       std::ratio<1>, std::ratio<0>, std::ratio<0>>> {
        using Named = Temperature;
};

template <> struct std::formatter<Temperature> : std::formatter<double> {
        auto format(const Temperature& quantity, std::format_context& ctx) const {
            return std::format_to(ctx.out(), "{}_k", quantity.internal());
        }
};

inline std::ostream& operator<<(std::ostream& os, const Temperature& quantity) {
    os << quantity.internal() << " k";
    return os;
}

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

constexpr inline Temperature from_kelvin(Number value) { return Temperature(value.internal()); }

constexpr inline double to_kelvin(Temperature quantity) { return quantity.internal(); }

constexpr inline Temperature from_celsius(Number value) { return Temperature(value.internal() + 273.15); }

constexpr inline double to_celsius(Temperature quantity) { return quantity.internal() - 273.15; }

constexpr inline Temperature from_fahrenheit(Number value) {
    return Temperature((value.internal() - 32) * (5.0 / 9.0) + 273.15);
}

constexpr inline double to_fahrenheit(Temperature quantity) {
    return (quantity.internal() - 273.15) * (9.0 / 5.0) + 32;
}

} // namespace units