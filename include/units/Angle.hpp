#pragma once
#include "units/units.hpp"

using Angle = Quantity<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<1>>;

constexpr Angle rad = Angle(1.0);
constexpr Angle deg = Angle(M_PI / 180);
constexpr Angle rot = Angle(M_TWOPI);

NEW_QUANTITY(AngularVelocity, radps, 0, 0, -1, 0, 1)
NEW_QUANTITY_VALUE(AngularVelocity, degps, deg / sec)
NEW_QUANTITY_VALUE(AngularVelocity, rps, rot / sec)
NEW_QUANTITY_VALUE(AngularVelocity, rpm, rot / min)

NEW_QUANTITY(AngularAcceleration, radps2, 0, 0, -2, 0, 1)
NEW_QUANTITY_VALUE(AngularAcceleration, degps2, deg / sec / sec)
NEW_QUANTITY_VALUE(AngularAcceleration, rps2, rot / sec / sec)
NEW_QUANTITY_VALUE(AngularAcceleration, rpm2, rot / min / min)

NEW_QUANTITY(AngularJerk, radps3, 0, 0, -3, 0, 1)
NEW_QUANTITY_VALUE(AngularJerk, rps3, rot / sec / sec / sec)
NEW_QUANTITY_VALUE(AngularJerk, rpm3, rot / min / min / min)

// Angle declaration operators
constexpr Angle operator""_stDeg(long double value) { return static_cast<double>(value) * deg; }

constexpr Angle operator""_stDeg(unsigned long long value) { return static_cast<double>(value) * deg; }

constexpr Angle operator""_stRad(long double value) { return Angle(static_cast<double>(value)); }

constexpr Angle operator""_stRad(unsigned long long value) { return Angle(static_cast<double>(value)); }

constexpr Angle operator""_cDeg(long double value) { return static_cast<double>(value) * deg; }

constexpr Angle operator""_cDeg(unsigned long long value) { return static_cast<double>(value) * deg; }

constexpr Angle operator""_cRad(long double value) { return Angle(static_cast<double>(value)); }

constexpr Angle operator""_cRad(unsigned long long value) { return Angle(static_cast<double>(value)); }

// Angle functions
namespace units {
constexpr Number sin(const Angle& rhs) { return Number(std::sin(rhs.val())); }

constexpr Number cos(const Angle& rhs) { return Number(std::cos(rhs.val())); }

constexpr Number tan(const Angle& rhs) { return Number(std::tan(rhs.val())); }

template <isQuantity Q> constexpr Angle asin(const Q& rhs) { return Angle(std::asin(rhs.val())); }

template <isQuantity Q> constexpr Angle acos(const Q& rhs) { return Angle(std::acos(rhs.val())); }

template <isQuantity Q> constexpr Angle atan(const Q& rhs) { return Angle(std::atan(rhs.val())); }

template <isQuantity Q> constexpr Angle atan2(const Q& lhs, const Q& rhs) {
    return Angle(std::atan2(lhs.val(), rhs.val()));
}

static inline Angle constrainAngle360(Angle in) { return mod(in, rot); }

static inline Angle constrainAngle180(Angle in) {
    in = mod(in + 180 * deg, rot);
    return in < Angle(0) ? in + 180 * deg : in - 180 * deg;
}
} // namespace units

// Angle to/from operators
constexpr inline Angle from_sRad(double value) { return Angle(value); }

constexpr inline double to_sRad(Angle quantity) { return quantity.val(); }

constexpr inline Angle from_sdeg(double value) { return value * deg; }

constexpr inline double to_sDeg(Angle quantity) { return quantity.convert(deg); }

constexpr inline Angle from_cRad(double value) { return 90 * deg - Angle(value); }

constexpr inline double to_cRad(Angle quantity) { return quantity.val(); }

constexpr inline Angle from_cdeg(double value) { return (90 - value) * deg; }

constexpr inline double to_cDeg(Angle quantity) { return (90 * deg - quantity).convert(deg); }