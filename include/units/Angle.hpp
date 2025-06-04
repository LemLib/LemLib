#pragma once

#include "units/units.hpp"

class Angle : public Quantity<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<1>, std::ratio<0>,
                              std::ratio<0>, std::ratio<0>> {
    public:
        explicit constexpr Angle(double value)
            : Quantity<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<1>, std::ratio<0>,
                       std::ratio<0>, std::ratio<0>>(value) {}

        constexpr Angle(Quantity<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<1>,
                                 std::ratio<0>, std::ratio<0>, std::ratio<0>>
                            value)
            : Quantity<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<1>, std::ratio<0>,
                       std::ratio<0>, std::ratio<0>>(value) {};
};

template <> struct LookupName<Quantity<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<1>,
                                       std::ratio<0>, std::ratio<0>, std::ratio<0>>> {
        using Named = Angle;
};

template <> struct std::formatter<Angle> : std::formatter<double> {
        auto format(const Angle& number, std::format_context& ctx) const {
            auto formatted_double = std::formatter<double>::format(number.internal(), ctx);
            return std::format_to(formatted_double, "_stRad");
        }
};

inline std::ostream& operator<<(std::ostream& os, const Angle& quantity) {
    os << quantity.internal() << " rad";
    return os;
}

/**
 * @brief DO NOT USE
 *
 * this class prevents conversion errors from compass angles to angles in standard position.
 *
 * consider the following:
 * 0_cDeg gets converted to standard position angles internally, so it's converted to 90
 * -0_cDeg is converted to standard position angles internally, and only afterwards is the
 * negative applied, so now it equals -90 internally
 *
 * This class solves this problem by introducing the CAngle type. You can do things like
 * negate it, multiply it, etc. without messing up the angle. However, this class can
 * only be created through string literals, you can't do something like
 * CAngle angle = 2_cDeg;
 * because the constructor is private. However, you can do
 * Angle angle = 2_cDeg;
 */
class CAngle {
        // make string literals friends, so they have access to the constructor
        friend constexpr CAngle operator""_cRad(long double value);
        friend constexpr CAngle operator""_cRad(unsigned long long value);
        friend constexpr CAngle operator""_cDeg(long double value);
        friend constexpr CAngle operator""_cDeg(unsigned long long value);
        friend constexpr CAngle operator""_cRot(long double value);
        friend constexpr CAngle operator""_cRot(unsigned long long value);
    public:
        // we don't want CAngle to have move, copy, or assignment operators
        constexpr CAngle& operator=(const CAngle&) = delete;
        constexpr CAngle(const CAngle&) = delete;

        // make CAngle able to be implicitly converted to Angle
        constexpr operator Angle() const { return Angle(M_PI_2 - this->value); }

        constexpr CAngle operator-() const { return CAngle(-this->value); }

        constexpr CAngle operator+() const { return CAngle(this->value); }
    private:
        const double value;

        constexpr CAngle(double value) : value(value) {}
};

/**
 * @brief Angle Distance class
 *
 * yet another helper class to manage the compass angle fiasco (it's getting nuked on May 15 2025)
 *
 * consider the following:
 * Angle exitRange1 = 0_cDeg;
 * Angle exitRange2 = 0_stDeg;
 *
 * It is expected that exitRange1 and exitRange2 is equal to each other.
 * However, this is not the case. 0_cDeg gets converted to 90_stDeg
 * implicitly. So, yet another helper class is necessary (hooray)
 *
 */
class AngleRange : public Angle {
    public:
        explicit constexpr AngleRange(double value) : Angle(fabs(value)) {}

        constexpr AngleRange(Angle value) : Angle(units::abs(value)) {}

        constexpr AngleRange(CAngle value) : Angle(units::abs(Angle(value) - Angle(M_PI_2))) {}
};

constexpr bool operator==(Angle lhs, CAngle rhs) { return lhs == Angle(rhs); }

constexpr Angle rad = Angle(1.0);
constexpr Angle deg = Angle(M_PI / 180);
constexpr Angle rot = Angle(M_TWOPI);

NEW_UNIT(AngularVelocity, radps, 0, 0, -1, 0, 1, 0, 0, 0)
NEW_UNIT_LITERAL(AngularVelocity, degps, deg / sec)
NEW_UNIT_LITERAL(AngularVelocity, rps, rot / sec)
NEW_UNIT_LITERAL(AngularVelocity, rpm, rot / min)

NEW_UNIT(AngularAcceleration, radps2, 0, 0, -2, 0, 1, 0, 0, 0)
NEW_UNIT_LITERAL(AngularAcceleration, degps2, deg / sec / sec)
NEW_UNIT_LITERAL(AngularAcceleration, rps2, rot / sec / sec)
NEW_UNIT_LITERAL(AngularAcceleration, rpm2, rot / min / min)

NEW_UNIT(AngularJerk, radps3, 0, 0, -3, 0, 1, 0, 0, 0)
NEW_UNIT_LITERAL(AngularJerk, rps3, rot / sec / sec / sec)
NEW_UNIT_LITERAL(AngularJerk, rpm3, rot / min / min / min)

// Angle declaration operators
// Standard orientation
constexpr Angle operator""_stRad(long double value) { return Angle(static_cast<double>(value)); }

constexpr Angle operator""_stRad(unsigned long long value) { return Angle(static_cast<double>(value)); }

constexpr Angle operator""_stDeg(long double value) { return static_cast<double>(value) * deg; }

constexpr Angle operator""_stDeg(unsigned long long value) { return static_cast<double>(value) * deg; }

constexpr Angle operator""_stRot(long double value) { return static_cast<double>(value) * rot; }

constexpr Angle operator""_stRot(unsigned long long value) { return static_cast<double>(value) * rot; }

// Compass orientation
constexpr CAngle operator""_cRad(long double value) { return CAngle(static_cast<double>(value)); }

constexpr CAngle operator""_cRad(unsigned long long value) { return CAngle(static_cast<double>(value)); }

constexpr CAngle operator""_cDeg(long double value) { return CAngle(static_cast<double>(value) * deg.internal()); }

constexpr CAngle operator""_cDeg(unsigned long long value) {
    return CAngle(static_cast<double>(value) * deg.internal());
}

constexpr CAngle operator""_cRot(long double value) { return CAngle(static_cast<double>(value) * rot.internal()); }

constexpr CAngle operator""_cRot(unsigned long long value) {
    return CAngle(static_cast<double>(value) * rot.internal());
}

// Angle functions
namespace units {
constexpr Number sin(const Angle& rhs) { return Number(std::sin(rhs.internal())); }

constexpr Number cos(const Angle& rhs) { return Number(std::cos(rhs.internal())); }

constexpr Number tan(const Angle& rhs) { return Number(std::tan(rhs.internal())); }

template <isQuantity Q> constexpr Angle asin(const Q& rhs) { return Angle(std::asin(rhs.internal())); }

template <isQuantity Q> constexpr Angle acos(const Q& rhs) { return Angle(std::acos(rhs.internal())); }

template <isQuantity Q> constexpr Angle atan(const Q& rhs) { return Angle(std::atan(rhs.internal())); }

template <isQuantity Q> constexpr Angle atan2(const Q& lhs, const Q& rhs) {
    return Angle(std::atan2(lhs.internal(), rhs.internal()));
}

static inline Angle constrainAngle360(Angle in) { return mod(in, rot); }

static inline Angle constrainAngle180(Angle in) {
    in = mod(in + 180 * deg, rot);
    return in < Angle(0) ? in + 180 * deg : in - 180 * deg;
}
} // namespace units

// Angle to/from operators
// Standard orientation
constexpr inline Angle from_stRad(Number value) { return Angle(value.internal()); }

constexpr inline double to_stRad(Angle quantity) { return quantity.internal(); }

constexpr inline Angle from_stDeg(Number value) { return value * deg; }

constexpr inline double to_stDeg(Angle quantity) { return quantity.convert(deg); }

constexpr inline Angle from_stRot(Number value) { return value * rot; }

constexpr inline double to_stRot(Angle quantity) { return quantity.convert(rot); }

// Compass orientation
constexpr inline Angle from_cRad(Number value) { return 90 * deg - Angle(value.internal()); }

constexpr inline double to_cRad(Angle quantity) { return quantity.internal(); }

constexpr inline Angle from_cDeg(Number value) { return (90 - value.internal()) * deg; }

constexpr inline double to_cDeg(Angle quantity) { return (90 * deg - quantity).convert(deg); }

constexpr inline Angle from_cRot(Number value) { return (90 - value.internal()) * deg; }

constexpr inline double to_cRot(Angle quantity) { return (90 * deg - quantity).convert(rot); }