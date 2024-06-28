#pragma once

#include <cmath>
#include <ratio>
#include <iostream>

// define M_PI if not already defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// define typenames

/**
 * @brief Quantity class
 *
 * This class is a template class that represents a quantity with a value and units.
 *
 * @tparam TYPENAMES the types of the units
 */
template <typename Mass = std::ratio<0>, typename Length = std::ratio<0>, typename Time = std::ratio<0>,
          typename Current = std::ratio<0>, typename Angle = std::ratio<0>, typename Temperature = std::ratio<0>,
          typename Luminosity = std::ratio<0>, typename Moles = std::ratio<0>>
class Quantity {
    protected:
        double value; /** the value stored in its base unit type */
    public:
        typedef Mass mass; /** mass unit type */
        typedef Length length; /** length unit type */
        typedef Time time; /** time unit type */
        typedef Current current; /** current unit type */
        typedef Angle angle; /** angle unit type */
        typedef Temperature temperature; /** temperature unit type */
        typedef Luminosity luminosity; /** luminosity unit type */
        typedef Moles moles; /** moles unit type */

        using Self = Quantity<Mass, Length, Time, Current, Angle, Temperature, Luminosity, Moles>;

        /**
         * @brief construct a new Quantity object
         *
         * This constructor initializes the value to 0
         */
        explicit constexpr Quantity() : value(0) {}

        /**
         * @brief construct a new Quantity object
         *
         * @param value the value to initialize the quantity with
         */
        explicit constexpr Quantity(double value) : value(value) {}

        /**
         * @brief construct a new Quantity object
         *
         * @param other the quantity to copy
         */
        constexpr Quantity(Self const& other) : value(other.value) {}

        /**
         * @brief get the value of the quantity in its base unit type
         *
         * @return constexpr double
         */
        constexpr double internal() const { return value; }

        // TODO: document this
        constexpr double convert(Self quantity) { return value / quantity.value; }

        /**
         * @brief set the value of this quantity to its current value plus another quantity
         *
         * @param other the quantity to add
         */
        constexpr void operator+=(Self other) { value += other.value; }

        /**
         * @brief set the value of this quantity to its current value minus another quantity
         *
         * @param other the quantity to subtract
         */
        constexpr void operator-=(Self other) { value -= other.value; }

        /**
         * @brief set the value of this quantity to its current value times a double
         *
         * @param multiple the multiple to multiply by
         */
        constexpr void operator*=(double multiple) { value *= multiple; }

        /**
         * @brief set the value of this quantity to its current value divided by a double
         *
         * @param dividend the dividend to divide by
         */
        constexpr void operator/=(double dividend) { value /= dividend; }

        /**
         * @brief set the value of this quantity to a double, only if the quantity is a number
         *
         * @param rhs the double to assign
         */
        constexpr void operator=(const double& rhs) {
            static_assert(std::ratio_equal<mass, std::ratio<0>>() && std::ratio_equal<length, std::ratio<0>>() &&
                              std::ratio_equal<time, std::ratio<0>>() && std::ratio_equal<current, std::ratio<0>>() &&
                              std::ratio_equal<angle, std::ratio<0>>() &&
                              std::ratio_equal<temperature, std::ratio<0>>() &&
                              std::ratio_equal<luminosity, std::ratio<0>>() && std::ratio_equal<moles, std::ratio<0>>(),
                          "Cannot assign a double directly to a non-number unit type");
            value = rhs;
        }
};

// quantity checker. Used by the isQuantity concept
template <typename Mass = std::ratio<0>, typename Length = std::ratio<0>, typename Time = std::ratio<0>,
          typename Current = std::ratio<0>, typename Angle = std::ratio<0>, typename Temperature = std::ratio<0>,
          typename Luminosity = std::ratio<0>, typename Moles = std::ratio<0>>
void quantityChecker(Quantity<Mass, Length, Time, Current, Angle, Temperature, Luminosity, Moles> q) {}

// isQuantity concept
template <typename Q>
concept isQuantity = requires(Q q) { quantityChecker(q); };

// Un(type)safely coerce the a unit into a different unit
template <isQuantity Q1, isQuantity Q2> constexpr inline Q1 unit_cast(Q2 quantity) { return Q1(quantity.internal()); }

template <isQuantity Q1, isQuantity Q2> using Multiplied = Quantity<
    std::ratio_add<typename Q1::mass, typename Q2::mass>, std::ratio_add<typename Q1::length, typename Q2::length>,
    std::ratio_add<typename Q1::time, typename Q2::time>, std::ratio_add<typename Q1::current, typename Q2::current>,
    std::ratio_add<typename Q1::angle, typename Q2::angle>,
    std::ratio_add<typename Q1::temperature, typename Q2::temperature>,
    std::ratio_add<typename Q1::luminosity, typename Q2::luminosity>,
    std::ratio_add<typename Q1::moles, typename Q2::moles>>;

template <isQuantity Q1, isQuantity Q2> using Divided =
    Quantity<std::ratio_subtract<typename Q1::mass, typename Q2::mass>,
             std::ratio_subtract<typename Q1::length, typename Q2::length>,
             std::ratio_subtract<typename Q1::time, typename Q2::time>,
             std::ratio_subtract<typename Q1::current, typename Q2::current>,
             std::ratio_subtract<typename Q1::angle, typename Q2::angle>,
             std::ratio_subtract<typename Q1::temperature, typename Q2::temperature>,
             std::ratio_subtract<typename Q1::luminosity, typename Q2::luminosity>,
             std::ratio_subtract<typename Q1::moles, typename Q2::moles>>;

template <isQuantity Q, typename factor> using Exponentiated =
    Quantity<std::ratio_multiply<typename Q::mass, factor>, std::ratio_multiply<typename Q::length, factor>,
             std::ratio_multiply<typename Q::time, factor>, std::ratio_multiply<typename Q::current, factor>,
             std::ratio_multiply<typename Q::angle, factor>, std::ratio_multiply<typename Q::temperature, factor>,
             std::ratio_multiply<typename Q::luminosity, factor>, std::ratio_multiply<typename Q::moles, factor>>;

template <isQuantity Q, typename quotient> using Rooted =
    Quantity<std::ratio_divide<typename Q::mass, quotient>, std::ratio_divide<typename Q::length, quotient>,
             std::ratio_divide<typename Q::time, quotient>, std::ratio_divide<typename Q::current, quotient>,
             std::ratio_divide<typename Q::angle, quotient>, std::ratio_divide<typename Q::temperature, quotient>,
             std::ratio_divide<typename Q::luminosity, quotient>, std::ratio_divide<typename Q::moles, quotient>>;

template <isQuantity Q> constexpr Q operator+(Q lhs, Q rhs) { return Q(lhs.internal() + rhs.internal()); }

template <isQuantity Q> constexpr Q operator-(Q lhs, Q rhs) { return Q(lhs.internal() - rhs.internal()); }

template <isQuantity Q> constexpr Q operator*(Q quantity, double multiple) { return Q(quantity.internal() * multiple); }

template <isQuantity Q> constexpr Q operator*(double multiple, Q quantity) { return Q(quantity.internal() * multiple); }

template <isQuantity Q> constexpr Q operator/(Q quantity, double divisor) { return Q(quantity.internal() / divisor); }

template <isQuantity Q1, isQuantity Q2, isQuantity Q3 = Multiplied<Q1, Q2>> Q3 constexpr operator*(Q1 lhs, Q2 rhs) {
    return Q3(lhs.internal() * rhs.internal());
}

template <isQuantity Q1, isQuantity Q2, isQuantity Q3 = Divided<Q1, Q2>> Q3 constexpr operator/(Q1 lhs, Q2 rhs) {
    return Q3(lhs.internal() / rhs.internal());
}

template <isQuantity Q> constexpr bool operator==(const Q& lhs, const Q& rhs) {
    return (lhs.internal() == rhs.internal());
}

template <isQuantity Q> constexpr bool operator!=(const Q& lhs, const Q& rhs) {
    return (lhs.internal() != rhs.internal());
}

template <isQuantity Q> constexpr bool operator<=(const Q& lhs, const Q& rhs) {
    return (lhs.internal() <= rhs.internal());
}

template <isQuantity Q> constexpr bool operator>=(const Q& lhs, const Q& rhs) {
    return (lhs.internal() >= rhs.internal());
}

template <isQuantity Q> constexpr bool operator<(const Q& lhs, const Q& rhs) {
    return (lhs.internal() < rhs.internal());
}

template <isQuantity Q> constexpr bool operator>(const Q& lhs, const Q& rhs) {
    return (lhs.internal() > rhs.internal());
}

#define NEW_UNIT(Name, suffix, m, l, t, i, a, o, j, n)                                                                 \
    class Name : public Quantity<std::ratio<m>, std::ratio<l>, std::ratio<t>, std::ratio<i>, std::ratio<a>,            \
                                 std::ratio<o>, std::ratio<j>, std::ratio<n>> {                                        \
        public:                                                                                                        \
            explicit constexpr Name(double value)                                                                      \
                : Quantity<std::ratio<m>, std::ratio<l>, std::ratio<t>, std::ratio<i>, std::ratio<a>, std::ratio<o>,   \
                           std::ratio<j>, std::ratio<n>>(value) {}                                                     \
            constexpr Name(Quantity<std::ratio<m>, std::ratio<l>, std::ratio<t>, std::ratio<i>, std::ratio<a>,         \
                                    std::ratio<o>, std::ratio<j>, std::ratio<n>>                                       \
                               value)                                                                                  \
                : Quantity<std::ratio<m>, std::ratio<l>, std::ratio<t>, std::ratio<i>, std::ratio<a>, std::ratio<o>,   \
                           std::ratio<j>, std::ratio<n>>(value) {};                                                    \
    };                                                                                                                 \
    constexpr Name suffix = Name(1.0);                                                                                 \
    constexpr Name operator""_##suffix(long double value) {                                                            \
        return Name(Quantity<std::ratio<m>, std::ratio<l>, std::ratio<t>, std::ratio<i>, std::ratio<a>, std::ratio<o>, \
                             std::ratio<j>, std::ratio<n>>(static_cast<double>(value)));                               \
    }                                                                                                                  \
    constexpr Name operator""_##suffix(unsigned long long value) {                                                     \
        return Name(Quantity<std::ratio<m>, std::ratio<l>, std::ratio<t>, std::ratio<i>, std::ratio<a>, std::ratio<o>, \
                             std::ratio<j>, std::ratio<n>>(static_cast<double>(value)));                               \
    }                                                                                                                  \
    inline std::ostream& operator<<(std::ostream& os, const Name& quantity) {                                          \
        os << quantity.internal() << "_" << #suffix;                                                                   \
        return os;                                                                                                     \
    }                                                                                                                  \
    constexpr inline Name from_##suffix(double value) { return Name(value); }                                          \
    constexpr inline double to_##suffix(Name quantity) { return quantity.internal(); }

#define NEW_UNIT_LITERAL(Name, suffix, multiple)                                                                       \
    constexpr Name suffix = multiple;                                                                                  \
    constexpr Name operator""_##suffix(long double value) { return static_cast<double>(value) * multiple; }            \
    constexpr Name operator""_##suffix(unsigned long long value) { return static_cast<double>(value) * multiple; }     \
    constexpr inline Name from_##suffix(double value) { return value * multiple; }                                     \
    constexpr inline double to_##suffix(Name quantity) { return quantity.convert(multiple); }

#define NEW_METRIC_PREFIXES(Name, base)                                                                                \
    NEW_UNIT_LITERAL(Name, T##base, base * 1E12)                                                                       \
    NEW_UNIT_LITERAL(Name, G##base, base * 1E9)                                                                        \
    NEW_UNIT_LITERAL(Name, M##base, base * 1E6)                                                                        \
    NEW_UNIT_LITERAL(Name, k##base, base * 1E3)                                                                        \
    NEW_UNIT_LITERAL(Name, c##base, base / 1E2)                                                                        \
    NEW_UNIT_LITERAL(Name, m##base, base / 1E3)                                                                        \
    NEW_UNIT_LITERAL(Name, u##base, base / 1E6)                                                                        \
    NEW_UNIT_LITERAL(Name, n##base, base / 1E9)

NEW_UNIT(Number, num, 0, 0, 0, 0, 0, 0, 0, 0)
NEW_UNIT_LITERAL(Number, percent, num / 100.0);

NEW_UNIT(Mass, kg, 1, 0, 0, 0, 0, 0, 0, 0)
NEW_UNIT_LITERAL(Mass, g, kg / 1000)
NEW_UNIT_LITERAL(Mass, lb, g * 453.6)

NEW_UNIT(Time, sec, 0, 0, 1, 0, 0, 0, 0, 0)
NEW_METRIC_PREFIXES(Time, sec)
NEW_UNIT_LITERAL(Time, min, sec * 60)
NEW_UNIT_LITERAL(Time, hr, min * 60)
NEW_UNIT_LITERAL(Time, day, hr * 24)

NEW_UNIT(Length, m, 0, 1, 0, 0, 0, 0, 0, 0)
NEW_METRIC_PREFIXES(Length, m)
NEW_UNIT_LITERAL(Length, in, cm * 2.54)
NEW_UNIT_LITERAL(Length, ft, in * 12)
NEW_UNIT_LITERAL(Length, yd, ft * 3)
NEW_UNIT_LITERAL(Length, mi, ft * 5280)
NEW_UNIT_LITERAL(Length, tile, 600 * mm)

NEW_UNIT(Area, m2, 0, 2, 0, 0, 0, 0, 0, 0)
NEW_UNIT_LITERAL(Area, Tm2, Tm* Tm);
NEW_UNIT_LITERAL(Area, Gm2, Gm* Gm);
NEW_UNIT_LITERAL(Area, Mm2, Mm* Mm);
NEW_UNIT_LITERAL(Area, km2, km* km);
NEW_UNIT_LITERAL(Area, cm2, cm* cm);
NEW_UNIT_LITERAL(Area, mm2, mm* mm);
NEW_UNIT_LITERAL(Area, um2, um* um);
NEW_UNIT_LITERAL(Area, nm2, nm* nm);
NEW_UNIT_LITERAL(Area, in2, in* in)

NEW_UNIT(LinearVelocity, mps, 0, 1, -1, 0, 0, 0, 0, 0)
NEW_METRIC_PREFIXES(LinearVelocity, mps);
NEW_UNIT_LITERAL(LinearVelocity, mph, m / hr)
NEW_METRIC_PREFIXES(LinearVelocity, mph)
NEW_UNIT_LITERAL(LinearVelocity, inps, in / sec)
NEW_UNIT_LITERAL(LinearVelocity, miph, mi / hr)

NEW_UNIT(LinearAcceleration, mps2, 0, 1, -2, 0, 0, 0, 0, 0)
NEW_METRIC_PREFIXES(LinearAcceleration, mps2)
NEW_UNIT_LITERAL(LinearAcceleration, mph2, m / hr / hr)
NEW_METRIC_PREFIXES(LinearAcceleration, mph2)
NEW_UNIT_LITERAL(LinearAcceleration, inps2, in / sec / sec)
NEW_UNIT_LITERAL(LinearAcceleration, miph2, mi / hr / hr)

NEW_UNIT(LinearJerk, mps3, 0, 1, -3, 0, 0, 0, 0, 0)
NEW_METRIC_PREFIXES(LinearJerk, mps3)
NEW_UNIT_LITERAL(LinearJerk, mph3, m / (hr * hr * hr))
NEW_METRIC_PREFIXES(LinearJerk, mph3)
NEW_UNIT_LITERAL(LinearJerk, inps3, in / (sec * sec * sec))
NEW_UNIT_LITERAL(LinearJerk, miph3, mi / (hr * hr * hr))

NEW_UNIT(Curvature, radpm, 0, -1, 0, 0, 0, 0, 0, 0);

NEW_UNIT(Inertia, kgm2, 1, 2, 0, 0, 0, 0, 0, 0)

NEW_UNIT(Force, N, 1, 1, -2, 0, 0, 0, 0, 0)

NEW_UNIT(Torque, Nm, 1, 2, -2, 0, 0, 0, 0, 0)

NEW_UNIT(Power, watt, 1, 2, -3, 0, 0, 0, 0, 0)

NEW_UNIT(Current, amp, 0, 0, 0, 1, 0, 0, 0, 0)

NEW_UNIT(Charge, coulomb, 0, 0, 1, 1, 0, 0, 0, 0)

NEW_UNIT(Voltage, volt, 1, 2, -3, -1, 0, 0, 0, 0)
NEW_METRIC_PREFIXES(Voltage, volt);

NEW_UNIT(Resistance, ohm, 1, 2, -3, -2, 0, 0, 0, 0)
NEW_METRIC_PREFIXES(Resistance, ohm)

NEW_UNIT(Conductance, siemen, -1, -2, 3, 2, 0, 0, 0, 0)
NEW_METRIC_PREFIXES(Conductance, siemen);

NEW_UNIT(Luminosity, candela, 0, 0, 0, 0, 0, 0, 1, 0);

NEW_UNIT(Moles, mol, 0, 0, 0, 0, 0, 0, 0, 1);

namespace units {
template <isQuantity Q> constexpr Q abs(const Q& lhs) { return Q(std::abs(lhs.internal())); }

template <isQuantity Q> constexpr Q max(const Q& lhs, const Q& rhs) { return (lhs > rhs ? lhs : rhs); }

template <isQuantity Q> constexpr Q min(const Q& lhs, const Q& rhs) { return (lhs < rhs ? lhs : rhs); }

template <int R, isQuantity Q, isQuantity S = Exponentiated<Q, std::ratio<R>>> constexpr S pow(const Q& lhs) {
    return S(std::pow(lhs.internal(), R));
}

template <isQuantity Q, isQuantity S = Exponentiated<Q, std::ratio<2>>> constexpr S square(const Q& lhs) {
    return pow<2>(lhs);
}

template <isQuantity Q, isQuantity S = Exponentiated<Q, std::ratio<3>>> constexpr S cube(const Q& lhs) {
    return pow<3>(lhs);
}

template <int R, isQuantity Q, isQuantity S = Rooted<Q, std::ratio<R>>> constexpr S root(const Q& lhs) {
    return S(std::pow(lhs.internal(), 1.0 / R));
}

template <isQuantity Q, isQuantity S = Rooted<Q, std::ratio<2>>> constexpr S sqrt(const Q& lhs) { return root<2>(lhs); }

template <isQuantity Q, isQuantity S = Rooted<Q, std::ratio<3>>> constexpr S cbrt(const Q& lhs) { return root<3>(lhs); }

template <isQuantity Q> constexpr Q hypot(const Q& lhs, const Q& rhs) {
    return Q(std::hypot(lhs.internal(), rhs.internal()));
}

template <isQuantity Q> constexpr Q mod(const Q& lhs, const Q& rhs) {
    return Q(std::fmod(lhs.internal(), rhs.internal()));
}

template <isQuantity Q1, isQuantity Q2> constexpr Q1 copysign(const Q1& lhs, const Q2& rhs) {
    return Q1(std::copysign(lhs.internal(), rhs.internal()));
}

template <isQuantity Q> constexpr int sgn(const Q& lhs) { return lhs.internal() < 0 ? -1 : 1; }

template <isQuantity Q> constexpr bool signbit(const Q& lhs) { return std::signbit(lhs.internal()); }

template <isQuantity Q> constexpr Q clamp(const Q& lhs, const Q& lo, Q& hi) {
    return Q(std::clamp(lhs.internal(), lo.internal(), hi.internal()));
}

template <isQuantity Q> constexpr Q ceil(const Q& lhs, const Q& rhs) {
    return Q(std::ceil(lhs.internal() / rhs.internal()) * rhs.internal());
}

template <isQuantity Q> constexpr Q floor(const Q& lhs, const Q& rhs) {
    return Q(std::floor(lhs.internal() / rhs.internal()) * rhs.internal());
}

template <isQuantity Q> constexpr Q trunc(const Q& lhs, const Q& rhs) {
    return Q(std::trunc(lhs.internal() / rhs.internal()) * rhs.internal());
}

template <isQuantity Q> constexpr Q round(const Q& lhs, const Q& rhs) {
    return Q(std::round(lhs.internal() / rhs.internal()) * rhs.internal());
}
} // namespace units

// Convert an angular unit `Q` to a linear unit correctly;
// mostly useful for velocities
template <isQuantity Q> Quantity<typename Q::mass, typename Q::angle, typename Q::time, typename Q::current,
                                 typename Q::length, typename Q::temperature, typename Q::luminosity, typename Q::moles>
toLinear(Quantity<typename Q::mass, typename Q::length, typename Q::time, typename Q::current, typename Q::angle,
                  typename Q::temperature, typename Q::luminosity, typename Q::moles>
             angular,
         Length diameter) {
    return unit_cast<Quantity<typename Q::mass, typename Q::angle, typename Q::time, typename Q::current,
                              typename Q::length, typename Q::temperature, typename Q::luminosity, typename Q::moles>>(
        angular * (diameter / 2.0));
}

// Convert an linear unit `Q` to a angular unit correctly;
// mostly useful for velocities
template <isQuantity Q> Quantity<typename Q::mass, typename Q::angle, typename Q::time, typename Q::current,
                                 typename Q::length, typename Q::temperature, typename Q::luminosity, typename Q::moles>
toAngular(Quantity<typename Q::mass, typename Q::length, typename Q::time, typename Q::current, typename Q::angle,
                   typename Q::temperature, typename Q::luminosity, typename Q::moles>
              linear,
          Length diameter) {
    return unit_cast<Quantity<typename Q::mass, typename Q::angle, typename Q::time, typename Q::current,
                              typename Q::length, typename Q::temperature, typename Q::luminosity, typename Q::moles>>(
        linear / (diameter / 2.0));
}