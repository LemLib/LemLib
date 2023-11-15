#pragma once

// Don't crash your Mars Orbiter
// Coherent unit system based on SI

#include <cmath>
#include <ratio>
#include <algorithm>
#include <iostream>
#include <type_traits>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using Zero = std::ratio<0>;
using One = std::ratio<1>;

template <typename Mass, typename Length, typename Time, typename Angle, typename Tag = Zero> struct Dimensions {
        Mass mass;
        Length length;
        Time time;
        Angle angle;
        Tag tag;
};

// The units used internally are:
// Kilograms for Mass
// Meters for Length
// Seconds for Time
// Radians for Angle

template <typename Mass, typename Length, typename Time, typename Angle, typename Rep = double> class Quantity {
        double value;
    public:
        // A proxy to be able to extract dimension units with decltype
        // Not used at runtime
        inline static Dimensions<Mass, Length, Time, Angle> dim {};

        explicit constexpr inline Quantity()
            : value(0) {}

        explicit constexpr inline Quantity(double val)
            : value(val) {}

        constexpr inline double raw() { return value; }

        constexpr inline Quantity const& operator+=(const Quantity& rhs) {
            value += rhs.value;
            return *this;
        }

        constexpr inline Quantity const& operator-=(const Quantity& rhs) {
            value -= rhs.value;
            return *this;
        }

        constexpr inline Quantity operator-() { return Quantity(value * -1); }

        constexpr inline Quantity const& operator*=(const double rhs) {
            value *= rhs;
            return *this;
        }

        constexpr inline Quantity const& operator/=(const double rhs) {
            value /= rhs;
            return *this;
        }

        // Returns the value of the quantity in multiples of the specified unit
        constexpr inline double convert(const Quantity unit) const { return value / unit.raw(); }

        // returns the raw value of the quantity (this breaks the type-safety of the unit system)
        constexpr inline double raw() const { return value; }
};

// isQuantity make everything much more readable
template <typename... args> void QuantityChecker(Quantity<args...>) {}

template <typename T>
concept isQuantity = requires {
    QuantityChecker(std::declval<T>());
};

template <typename Q1, typename Q2> constexpr inline Q1 unit_cast(Q2 quantity) { return Q1(quantity.raw()); }

template <typename Q1, typename Q2> using Multiplication =
    Quantity<std::ratio_add<decltype(Q1::dim.mass), decltype(Q2::dim.mass)>,
             std::ratio_add<decltype(Q1::dim.length), decltype(Q2::dim.length)>,
             std::ratio_add<decltype(Q1::dim.time), decltype(Q2::dim.time)>,
             std::ratio_add<decltype(Q1::dim.angle), decltype(Q2::dim.angle)>>;

template <typename Q1, typename Q2> using Division =
    Quantity<std::ratio_subtract<decltype(Q1::dim.mass), decltype(Q2::dim.mass)>,
             std::ratio_subtract<decltype(Q1::dim.length), decltype(Q2::dim.length)>,
             std::ratio_subtract<decltype(Q1::dim.time), decltype(Q2::dim.time)>,
             std::ratio_subtract<decltype(Q1::dim.angle), decltype(Q2::dim.angle)>>;

#define QUANTITY_NEW(Name, base, Mass, Length, Time, Angle)                                                            \
    using Name = Quantity<std::ratio<Mass>, std::ratio<Length>, std::ratio<Time>, std::ratio<Angle>>;                  \
    constexpr inline Name base = Name(1);                                                                              \
    constexpr inline Name operator""_##base(long double value) { return Name(static_cast<double>(value)); }            \
    constexpr inline Name operator""_##base(unsigned long long value) { return Name(static_cast<double>(value)); }     \
    constexpr inline Name from_##base(double value) { return Name(value); }                                            \
    constexpr inline double to_##base(Name quantity) { return quantity.raw(); }                                        \
    inline std::ostream& operator<<(std::ostream& os, const Name& quantity) {                                          \
        os << quantity.raw() << "_" << #base;                                                                          \
        return os;                                                                                                     \
    }

#define QUANTITY_NEW_TAGGED(Name, base, Mass, Length, Time, Angle, Tag)                                                \
    using Name = Quantity<std::ratio<Mass>, std::ratio<Length>, std::ratio<Time>, std::ratio<Angle>, std::ratio<Tag>>; \
    constexpr inline Name base = Name(1);                                                                              \
    constexpr inline Name operator""_##base(long double value) { return Name(static_cast<double>(value)); }            \
    constexpr inline Name operator""_##base(unsigned long long value) { return Name(static_cast<double>(value)); }     \
    constexpr inline Name from_##base(double value) { return Name(value); }                                            \
    constexpr inline double to_##base(Name quantity) { return quantity.raw(); }                                        \
    inline std::ostream& operator<<(std::ostream& os, const Name& quantity) {                                          \
        os << quantity.raw() << "_" << #base;                                                                          \
        return os;                                                                                                     \
    }

#define QUANTITY_LIT(Name, suffix, ratio)                                                                              \
    constexpr inline Name suffix = ratio;                                                                              \
    constexpr inline Name operator""_##suffix(long double value) { return Name(static_cast<double>(value) * ratio); }  \
    constexpr inline Name operator""_##suffix(unsigned long long value) {                                              \
        return Name(static_cast<double>(value) * ratio);                                                               \
    }                                                                                                                  \
    constexpr inline Name from_##suffix(double value) { return value * ratio; }                                        \
    constexpr inline double to_##suffix(Name quantity) { return quantity.convert(ratio); }

QUANTITY_NEW(Number, num, 0, 0, 0, 0)

// Aritmetic operators
template <isQuantity Q> constexpr inline Q operator+(const Q& lhs, const Q& rhs) { return Q(lhs.raw() + rhs.raw()); }

template <isQuantity Q> constexpr inline Q operator-(const Q& lhs, const Q& rhs) { return Q(lhs.raw() - rhs.raw()); }

template <isQuantity Q1, isQuantity Q2>
constexpr inline Multiplication<Q1, Q2> operator*(const Q1& lhs, const Q2& rhs) {
    return Multiplication<Q1, Q2>(lhs.raw() * rhs.raw());
}

template <typename Q, typename = std::enable_if_t<std::is_class<Q>::value>>
constexpr inline Q operator*(const double& lhs, const Q& rhs) {
    return Q(lhs * rhs.raw());
}

template <typename Q, typename = std::enable_if_t<std::is_class<Q>::value>>
constexpr inline Q operator*(const Q& lhs, const double& rhs) {
    return Q(lhs.raw() * rhs);
}

template <typename Q1, typename Q2> constexpr inline Division<Q1, Q2> operator/(const Q1& lhs, const Q2& rhs) {
    return Division<Q1, Q2>(lhs.raw() / rhs.raw());
}

template <typename Q> constexpr inline Quantity<
    std::ratio_subtract<Zero, decltype(Q::dim.mass)>, std::ratio_subtract<Zero, decltype(Q::dim.length)>,
    std::ratio_subtract<Zero, decltype(Q::dim.time)>, std::ratio_subtract<Zero, decltype(Q::dim.angle)>>
operator/(const double& x, const Q& rhs) {
    return Quantity<
        std::ratio_subtract<Zero, decltype(Q::dim.mass)>, std::ratio_subtract<Zero, decltype(Q::dim.length)>,
        std::ratio_subtract<Zero, decltype(Q::dim.time)>, std::ratio_subtract<Zero, decltype(Q::dim.angle)>>(x /
                                                                                                             rhs.raw());
}

template <typename Q, typename = std::enable_if_t<std::is_class<Q>::value>>
constexpr inline Q operator/(const Q& rhs, const double& x) {
    return Q(rhs.raw() / x);
}

// Comparison operators
template <isQuantity Q> constexpr inline bool operator==(const Q& lhs, const Q& rhs) {
    return (lhs.raw() == rhs.raw());
}

template <isQuantity Q> constexpr inline bool operator!=(const Q& lhs, const Q& rhs) {
    return (lhs.raw() != rhs.raw());
}

template <isQuantity Q> constexpr inline bool operator<=(const Q& lhs, const Q& rhs) {
    return (lhs.raw() <= rhs.raw());
}

template <isQuantity Q> constexpr inline bool operator>=(const Q& lhs, const Q& rhs) {
    return (lhs.raw() >= rhs.raw());
}

template <isQuantity Q> constexpr inline bool operator<(const Q& lhs, const Q& rhs) { return (lhs.raw() < rhs.raw()); }

template <isQuantity Q> constexpr inline bool operator>(const Q& lhs, const Q& rhs) { return (lhs.raw() > rhs.raw()); }

namespace units {
template <isQuantity Q> constexpr inline Q abs(const Q& lhs) { return Q(std::abs(lhs.raw())); }

template <isQuantity Q> constexpr inline Q max(const Q& lhs, const Q& rhs) { return (lhs > rhs ? lhs : rhs); }

template <isQuantity Q> constexpr inline Q min(const Q& lhs, const Q& rhs) { return (lhs < rhs ? lhs : rhs); }

template <isQuantity R, isQuantity Q>
constexpr inline Quantity<std::ratio_multiply<decltype(Q::dim.mass), R>,
                          std::ratio_multiply<decltype(Q::dim.length), R>,
                          std::ratio_multiply<decltype(Q::dim.time), R>, std::ratio_multiply<decltype(Q::dim.angle), R>>
pow(const Q& lhs) {
    return Quantity<std::ratio_multiply<decltype(Q::dim.mass), R>, std::ratio_multiply<decltype(Q::dim.dlenth), R>,
                    std::ratio_multiply<decltype(Q::dim.time), R>, std::ratio_multiply<decltype(Q::dim.angle), R>>(
        std::pow(lhs.raw(), double(R::num) / R::den));
}

template <int R, isQuantity Q> constexpr inline Quantity<std::ratio_multiply<decltype(Q::dim.mass), std::ratio<R>>,
                                                         std::ratio_multiply<decltype(Q::dim.length), std::ratio<R>>,
                                                         std::ratio_multiply<decltype(Q::dim.time), std::ratio<R>>,
                                                         std::ratio_multiply<decltype(Q::dim.angle), std::ratio<R>>>
pow(const Quantity<decltype(Q::dim.mass), decltype(Q::dim.length), decltype(Q::dim.time), decltype(Q::dim.angle)>&
        lhs) {
    return Quantity<std::ratio_multiply<decltype(Q::dim.mass), std::ratio<R>>,
                    std::ratio_multiply<decltype(Q::dim.length), std::ratio<R>>,
                    std::ratio_multiply<decltype(Q::dim.time), std::ratio<R>>,
                    std::ratio_multiply<decltype(Q::dim.angle), std::ratio<R>>>(std::pow(lhs.raw(), R));
}

template <int R, isQuantity Q> constexpr inline Quantity<
    std::ratio_divide<decltype(Q::dim.mass), std::ratio<R>>, std::ratio_divide<decltype(Q::dim.length), std::ratio<R>>,
    std::ratio_divide<decltype(Q::dim.time), std::ratio<R>>, std::ratio_divide<decltype(Q::dim.angle), std::ratio<R>>>
root(const Quantity<decltype(Q::dim.mass), decltype(Q::dim.length), decltype(Q::dim.time), decltype(Q::dim.angle)>&
         lhs) {
    return Quantity<std::ratio_divide<decltype(Q::dim.mass), std::ratio<R>>,
                    std::ratio_divide<decltype(Q::dim.length), std::ratio<R>>,
                    std::ratio_divide<decltype(Q::dim.time), std::ratio<R>>,
                    std::ratio_divide<decltype(Q::dim.angle), std::ratio<R>>>(std::pow(lhs.raw(), 1.0 / R));
}

template <isQuantity Q> constexpr inline Quantity<
    std::ratio_divide<decltype(Q::dim.mass), std::ratio<2>>, std::ratio_divide<decltype(Q::dim.length), std::ratio<2>>,
    std::ratio_divide<decltype(Q::dim.time), std::ratio<2>>, std::ratio_divide<decltype(Q::dim.angle), std::ratio<2>>>
sqrt(const Q& rhs) {
    return Quantity<std::ratio_divide<decltype(Q::dim.mass), std::ratio<2>>,
                    std::ratio_divide<decltype(Q::dim.length), std::ratio<2>>,
                    std::ratio_divide<decltype(Q::dim.time), std::ratio<2>>,
                    std::ratio_divide<decltype(Q::dim.angle), std::ratio<2>>>(std::sqrt(rhs.raw()));
}

template <isQuantity Q> constexpr inline Quantity<
    std::ratio_divide<decltype(Q::dim.mass), std::ratio<3>>, std::ratio_divide<decltype(Q::dim.length), std::ratio<3>>,
    std::ratio_divide<decltype(Q::dim.time), std::ratio<3>>, std::ratio_divide<decltype(Q::dim.angle), std::ratio<3>>>
cbrt(const Q& rhs) {
    return Quantity<std::ratio_divide<decltype(Q::dim.mass), std::ratio<3>>,
                    std::ratio_divide<decltype(Q::dim.length), std::ratio<3>>,
                    std::ratio_divide<decltype(Q::dim.time), std::ratio<3>>,
                    std::ratio_divide<decltype(Q::dim.angle), std::ratio<3>>>(std::cbrt(rhs.raw()));
}

template <isQuantity Q> constexpr inline Quantity<std::ratio_multiply<decltype(Q::dim.mass), std::ratio<2>>,
                                                  std::ratio_multiply<decltype(Q::dim.length), std::ratio<2>>,
                                                  std::ratio_multiply<decltype(Q::dim.time), std::ratio<2>>,
                                                  std::ratio_multiply<decltype(Q::dim.angle), std::ratio<2>>>
square(const Q& rhs) {
    return Quantity<std::ratio_multiply<decltype(Q::dim.mass), std::ratio<2>>,
                    std::ratio_multiply<decltype(Q::dim.length), std::ratio<2>>,
                    std::ratio_multiply<decltype(Q::dim.time), std::ratio<2>>,
                    std::ratio_multiply<decltype(Q::dim.angle), std::ratio<2>>>(std::pow(rhs.raw(), 2));
}

template <isQuantity Q> constexpr inline Quantity<std::ratio_multiply<decltype(Q::dim.mass), std::ratio<3>>,
                                                  std::ratio_multiply<decltype(Q::dim.length), std::ratio<3>>,
                                                  std::ratio_multiply<decltype(Q::dim.time), std::ratio<3>>,
                                                  std::ratio_multiply<decltype(Q::dim.angle), std::ratio<3>>>
cube(const Q& rhs) {
    return Quantity<std::ratio_multiply<decltype(Q::dim.mass), std::ratio<3>>,
                    std::ratio_multiply<decltype(Q::dim.length), std::ratio<3>>,
                    std::ratio_multiply<decltype(Q::dim.time), std::ratio<3>>,
                    std::ratio_multiply<decltype(Q::dim.angle), std::ratio<3>>>(std::pow(rhs.raw(), 3));
}

template <isQuantity Q> constexpr inline Q hypot(const Q& lhs, const Q& rhs) {
    return Q(std::hypot(lhs.raw(), rhs.raw()));
}

template <isQuantity Q> constexpr inline Q mod(const Q& lhs, const Q& rhs) {
    return Q(std::fmod(lhs.raw(), rhs.raw()));
}

template <isQuantity Q1, isQuantity Q2> constexpr inline Q1 copysign(const Q1& lhs, const Q2& rhs) {
    return Q1(std::copysign(lhs.raw(), rhs.raw()));
}

template <isQuantity Q> constexpr inline int sgn(const Q& lhs) { return lhs.raw() < 0 ? -1 : 1; }

template <isQuantity Q> constexpr inline bool signbit(const Q& lhs) { return std::signbit(lhs.raw()); }

template <isQuantity Q> constexpr inline Q clamp(const Q& lhs, const Q& lo, Q& hi) {
    return Q(std::clamp(lhs.raw(), lo.raw(), hi.raw()));
}

template <isQuantity Q> constexpr inline Q ceil(const Q& lhs, const Q& rhs) {
    return Q(std::ceil(lhs.raw() / rhs.raw()) * rhs.raw());
}

template <isQuantity Q> constexpr inline Q floor(const Q& lhs, const Q& rhs) {
    return Q(std::floor(lhs.raw() / rhs.raw()) * rhs.raw());
}

template <isQuantity Q> constexpr inline Q trunc(const Q& lhs, const Q& rhs) {
    return Q(std::trunc(lhs.raw() / rhs.raw()) * rhs.raw());
}

template <isQuantity Q> constexpr inline Q round(const Q& lhs, const Q& rhs) {
    return Q(std::round(lhs.raw() / rhs.raw()) * rhs.raw());
}

using isAngle = Quantity<Zero, Zero, Zero, One>;

constexpr inline Number sin(const isAngle& rhs) { return Number(std::sin(rhs.raw())); }

constexpr inline Number cos(isAngle& rhs) { return Number(std::cos(rhs.raw())); }

constexpr inline Number tan(isAngle& rhs) { return Number(std::tan(rhs.raw())); }

template <isQuantity Q> constexpr inline isAngle asin(const Q& rhs) { return isAngle(std::asin(rhs.raw())); }

template <isQuantity Q> constexpr inline isAngle acos(const Q& rhs) { return isAngle(std::acos(rhs.raw())); }

template <isQuantity Q> constexpr inline isAngle atan(const Q& rhs) { return isAngle(std::atan(rhs.raw())); }

constexpr inline Number sinh(isAngle rhs) { return Number(std::sinh(rhs.raw())); }

constexpr inline Number cosh(isAngle rhs) { return Number(std::cosh(rhs.raw())); }

constexpr inline Number tanh(isAngle rhs) { return Number(std::tanh(rhs.raw())); }

template <isQuantity Q> constexpr inline isAngle asinh(const Q& rhs) { return isAngle(std::asinh(rhs.raw())); }

template <isQuantity Q> constexpr inline isAngle acosh(const Q& rhs) { return isAngle(std::acosh(rhs.raw())); }

template <isQuantity Q> constexpr inline isAngle atanh(const Q& rhs) { return isAngle(std::atanh(rhs.raw())); }

template <isQuantity Q> constexpr inline isAngle atan2(const Q& lhs, const Q& rhs) {
    return isAngle(std::atan2(lhs.raw(), rhs.raw()));
}

} // namespace units

QUANTITY_NEW(Time, sec, 0, 0, 1, 0)
QUANTITY_LIT(Time, ms, sec / 1000)
QUANTITY_LIT(Time, min, sec * 60)
QUANTITY_LIT(Time, hr, min * 60)
QUANTITY_LIT(Time, day, hr * 24)

QUANTITY_NEW(Length, m, 0, 1, 0, 0)
QUANTITY_LIT(Length, mm, m / 1000)
QUANTITY_LIT(Length, cm, m / 100)
QUANTITY_LIT(Length, km, m * 1000)
QUANTITY_LIT(Length, in, cm * 2.54)
QUANTITY_LIT(Length, ft, in * 12)
QUANTITY_LIT(Length, yd, ft * 3)
QUANTITY_LIT(Length, mi, ft * 5280)
QUANTITY_LIT(Length, tiles, 600 * mm)

QUANTITY_NEW(Area, m2, 0, 2, 0, 0)

QUANTITY_NEW(Angle, rad, 0, 0, 0, 1)
QUANTITY_LIT(Angle, deg, (M_PI / 180) * rad)
QUANTITY_LIT(Angle, rot, 360 * deg)

QUANTITY_NEW(LinearVelocity, mps, 0, 1, -1, 0)
QUANTITY_LIT(LinearVelocity, cmps, cm / sec)
QUANTITY_LIT(LinearVelocity, inps, in / sec)
QUANTITY_LIT(LinearVelocity, miph, mi / hr)
QUANTITY_LIT(LinearVelocity, kmph, km / hr)

QUANTITY_NEW(AngularVelocity, radps, 0, 0, -1, 1)
QUANTITY_LIT(AngularVelocity, degps, deg / sec)
QUANTITY_LIT(AngularVelocity, rps, rot / sec)
QUANTITY_LIT(AngularVelocity, rpm, rot / min)

QUANTITY_NEW(LinearAcceleration, mps2, 0, 1, -2, 0)
QUANTITY_LIT(LinearAcceleration, cmps2, cm / sec / sec)
QUANTITY_LIT(LinearAcceleration, inps2, in / sec / sec)
QUANTITY_LIT(LinearAcceleration, miph2, mi / hr / hr)
QUANTITY_LIT(LinearAcceleration, kmph2, km / hr / hr)

QUANTITY_NEW(AngularAcceleration, radps2, 0, 0, -2, 1)
QUANTITY_LIT(AngularAcceleration, degps2, deg / sec / sec)
QUANTITY_LIT(AngularAcceleration, rps2, rot / sec / sec)
QUANTITY_LIT(AngularAcceleration, rpm2, rot / min / min)

QUANTITY_NEW(LinearJerk, mps3, 0, 1, -3, 0)
QUANTITY_LIT(LinearJerk, cmps3, cm / sec / sec / sec)
QUANTITY_LIT(LinearJerk, inps3, in / sec / sec / sec)
QUANTITY_LIT(LinearJerk, miph3, mi / hr / hr / hr)
QUANTITY_LIT(LinearJerk, kmph3, km / hr / hr / hr)

QUANTITY_NEW(AngularJerk, radps3, 0, 0, -3, 1)
QUANTITY_LIT(AngularJerk, rps3, rot / sec / sec / sec)
QUANTITY_LIT(AngularJerk, rpm3, rot / min / min / min)

QUANTITY_NEW(Curvature, radpm, 0, -1, 0, 1);

QUANTITY_NEW(Radius, mprad, 0, 1, 0, -1);

QUANTITY_NEW_TAGGED(Temperature, celcius, 0, 0, 0, 0, 0)

QUANTITY_NEW_TAGGED(Voltage, volts, 0, 0, 0, 0, 1)
QUANTITY_LIT(Voltage, mvolts, volts / 1000)
QUANTITY_LIT(Voltage, conunits, (volts / 12000) * 127)

template <typename Q>
Quantity<decltype(Q::dim.mass), decltype(Q::dim.angle), decltype(Q::dim.time), decltype(Q::dim.length)> to_linear(
    Quantity<decltype(Q::dim.mass), decltype(Q::dim.length), decltype(Q::dim.time), decltype(Q::dim.angle)> angular,
    Length diameter) {
    return unit_cast<
        Quantity<decltype(Q::dim.mass), decltype(Q::dim.angle), decltype(Q::dim.time), decltype(Q::dim.length)>>(
        angular * (diameter / 2.0));
}

template <typename Q>
Quantity<decltype(Q::dim.mass), decltype(Q::dim.angle), decltype(Q::dim.time), decltype(Q::dim.length)> to_angular(
    Quantity<decltype(Q::dim.mass), decltype(Q::dim.length), decltype(Q::dim.time), decltype(Q::dim.angle)> linear,
    Length diameter) {
    return unit_cast<
        Quantity<decltype(Q::dim.mass), decltype(Q::dim.angle), decltype(Q::dim.time), decltype(Q::dim.length)>>(
        linear / (diameter / 2.0));
}

constexpr Time FOREVER = Time(std::numeric_limits<double>::infinity());