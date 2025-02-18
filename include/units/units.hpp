#pragma once

#include <algorithm>
#include <array>
#include <cmath>
#include <format>
#include <iostream>
#include <ratio>
#include <type_traits>
#include <utility>

// define M_PI if not already defined
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// define typenames

/**
 * @brief Quantity class
 *
 * This class is a template class that represents a quantity with a value and
 * units.
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

        using Dimensionless = Quantity<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>,
                                       std::ratio<0>, std::ratio<0>, std::ratio<0>>;

        /**
         * @brief construct a new Quantity object
         *
         * This constructor initializes the value to 0
         */
        explicit constexpr Quantity()
            : value(0) {}

        /**
         * @brief construct a new Quantity object
         *
         * @param value the value to initialize the quantity with
         */
        explicit constexpr Quantity(double value)
            requires(!std::is_same_v<Self, Dimensionless>)
            : value(value) {}

        constexpr Quantity(double value)
            requires std::is_same_v<Self, Dimensionless>
            : value(value) {}

        constexpr operator double() const
            requires std::is_same_v<Self, Dimensionless>
        {
            return value;
        }

        /**
         * @brief construct a new Quantity object
         *
         * @param other the quantity to copy
         */
        constexpr Quantity(const Self& other)
            : value(other.value) {}

        /**
         * @brief get the value of the quantity in its base unit type
         *
         * @return constexpr double
         */
        constexpr double internal() const { return value; }

        // TODO: document this
        constexpr double convert(Self quantity) const { return value / quantity.value; }

        /**
         * @brief set the value of this quantity to its current value plus another
         * quantity
         *
         * @param other the quantity to add
         */
        constexpr void operator+=(Self other) { value += other.value; }

        constexpr void operator+=(double other)
            requires std::is_same_v<Self, Dimensionless>
        {
            value += other;
        }

        /**
         * @brief set the value of this quantity to its current value minus another
         * quantity
         *
         * @param other the quantity to subtract
         */
        constexpr void operator-=(Self other) { value -= other.value; }

        constexpr void operator-=(double other)
            requires std::is_same_v<Self, Dimensionless>
        {
            value -= other;
        }

        /**
         * @brief set the value of this quantity to its current value times a double
         *
         * @param multiple the multiple to multiply by
         */
        constexpr void operator*=(double multiple) { value *= multiple; }

        constexpr void operator*=(double other)
            requires std::is_same_v<Self, Dimensionless>
        {
            value *= other;
        }

        /**
         * @brief set the value of this quantity to its current value divided by a
         * double
         *
         * @param dividend the dividend to divide by
         */
        constexpr void operator/=(double dividend) { value /= dividend; }

        constexpr void operator/=(double other)
            requires std::is_same_v<Self, Dimensionless>
        {
            value /= other;
        }

        /**
         * @brief set the value of this quantity to a double, only if the quantity
         * is a number
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

/* Number is a special type, because it can be implicitly converted to and from
 * any arithmetic type */
class Number : public Quantity<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>,
                               std::ratio<0>, std::ratio<0>> {
    public:
        constexpr Number(double number)
            : Quantity<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>,
                       std::ratio<0>, std::ratio<0>>(number) {}
};

template <typename Q> struct LookupName {
        using Named = Q;
};

template <typename Q> using Named = typename LookupName<Q>::Named;

// quantity checker. Used by the isQuantity concept
template <typename Mass = std::ratio<0>, typename Length = std::ratio<0>, typename Time = std::ratio<0>,
          typename Current = std::ratio<0>, typename Angle = std::ratio<0>, typename Temperature = std::ratio<0>,
          typename Luminosity = std::ratio<0>, typename Moles = std::ratio<0>>
void quantityChecker(Quantity<Mass, Length, Time, Current, Angle, Temperature, Luminosity, Moles>) {}

// isQuantity concept
template <typename Q>
concept isQuantity = requires(Q q) { quantityChecker(q); };

// Isomorphic concept - used to ensure unit equivalency
template <typename Q, typename... Quantities>
concept Isomorphic = ((std::convertible_to<Q, Quantities> && std::convertible_to<Quantities, Q>) && ...);

// Un(type)safely coerce the a unit into a different unit
template <isQuantity Q1, isQuantity Q2> inline constexpr Q1 unit_cast(Q2 quantity) { return Q1(quantity.internal()); }

template <isQuantity Q1, isQuantity Q2> using Multiplied = Named<Quantity<
    std::ratio_add<typename Q1::mass, typename Q2::mass>, std::ratio_add<typename Q1::length, typename Q2::length>,
    std::ratio_add<typename Q1::time, typename Q2::time>, std::ratio_add<typename Q1::current, typename Q2::current>,
    std::ratio_add<typename Q1::angle, typename Q2::angle>,
    std::ratio_add<typename Q1::temperature, typename Q2::temperature>,
    std::ratio_add<typename Q1::luminosity, typename Q2::luminosity>,
    std::ratio_add<typename Q1::moles, typename Q2::moles>>>;

template <isQuantity Q1, isQuantity Q2> using Divided =
    Named<Quantity<std::ratio_subtract<typename Q1::mass, typename Q2::mass>,
                   std::ratio_subtract<typename Q1::length, typename Q2::length>,
                   std::ratio_subtract<typename Q1::time, typename Q2::time>,
                   std::ratio_subtract<typename Q1::current, typename Q2::current>,
                   std::ratio_subtract<typename Q1::angle, typename Q2::angle>,
                   std::ratio_subtract<typename Q1::temperature, typename Q2::temperature>,
                   std::ratio_subtract<typename Q1::luminosity, typename Q2::luminosity>,
                   std::ratio_subtract<typename Q1::moles, typename Q2::moles>>>;

template <isQuantity Q, typename factor> using Exponentiated = Named<
    Quantity<std::ratio_multiply<typename Q::mass, factor>, std::ratio_multiply<typename Q::length, factor>,
             std::ratio_multiply<typename Q::time, factor>, std::ratio_multiply<typename Q::current, factor>,
             std::ratio_multiply<typename Q::angle, factor>, std::ratio_multiply<typename Q::temperature, factor>,
             std::ratio_multiply<typename Q::luminosity, factor>, std::ratio_multiply<typename Q::moles, factor>>>;

template <isQuantity Q, typename quotient> using Rooted = Named<
    Quantity<std::ratio_divide<typename Q::mass, quotient>, std::ratio_divide<typename Q::length, quotient>,
             std::ratio_divide<typename Q::time, quotient>, std::ratio_divide<typename Q::current, quotient>,
             std::ratio_divide<typename Q::angle, quotient>, std::ratio_divide<typename Q::temperature, quotient>,
             std::ratio_divide<typename Q::luminosity, quotient>, std::ratio_divide<typename Q::moles, quotient>>>;

template <isQuantity Q> struct std::formatter<Q> : std::formatter<double> {
        auto format(const Q& quantity, std::format_context& ctx) const {
            constinit static std::array<std::pair<intmax_t, intmax_t>, 8> dims {{
                {Q::mass::num, Q::mass::den},
                {Q::length::num, Q::length::den},
                {Q::time::num, Q::time::den},
                {Q::current::num, Q::current::den},
                {Q::angle::num, Q::angle::den},
                {Q::temperature::num, Q::temperature::den},
                {Q::luminosity::num, Q::luminosity::den},
                {Q::moles::num, Q::moles::den},
            }};
            std::array<const char*, 8> prefixes {"_kg", "_m", "_s", "_A", "_rad", "_K", "_cd", "_mol"};

            auto out = ctx.out();

            // Format the quantity value
            out = std::formatter<double>::format(quantity.internal(), ctx);

            // Add dimensions and prefixes
            for (size_t i = 0; i != 8; i++) {
                if (dims[i].first != 0) {
                    out = std::format_to(out, "{}", prefixes[i]);
                    if (dims[i].first != 1 || dims[i].second != 1) { out = std::format_to(out, "^{}", dims[i].first); }
                    if (dims[i].second != 1) { out = std::format_to(out, "/{}", dims[i].second); }
                }
            }

            return out;
        }
};

inline void unit_printer_helper(std::ostream& os, double quantity,
                                const std::array<std::pair<intmax_t, intmax_t>, 8>& dims) {
    static constinit std::array<const char*, 8> prefixes {"_kg", "_m", "_s", "_A", "_rad", "_K", "_cd", "_mol"};
    os << quantity;
    for (size_t i = 0; i != 8; i++) {
        if (dims[i].first != 0) {
            os << prefixes[i];
            if (dims[i].first != 1 || dims[i].second != 1) os << '^' << dims[i].first;
            if (dims[i].second != 1) os << '/' << dims[i].second;
        }
    }
}

template <isQuantity Q> inline std::ostream& operator<<(std::ostream& os, const Q& quantity) {
    if constexpr (!std::is_same_v<Named<Q>, Q>) {
        os << Named<Q>(quantity);
    } else {
        constinit static std::array<std::pair<intmax_t, intmax_t>, 8> dims {{
            {Q::mass::num, Q::mass::den},
            {Q::length::num, Q::length::den},
            {Q::time::num, Q::time::den},
            {Q::current::num, Q::current::den},
            {Q::angle::num, Q::angle::den},
            {Q::temperature::num, Q::temperature::den},
            {Q::luminosity::num, Q::luminosity::den},
            {Q::moles::num, Q::moles::den},
        }};
        unit_printer_helper(os, quantity.internal(), dims);
    }
    return os;
}

template <isQuantity Q> constexpr Q operator+(Q rhs) { return rhs; }

template <isQuantity Q, isQuantity R> constexpr Q operator+(Q lhs, R rhs)
    requires Isomorphic<Q, R>
{
    return Q(lhs.internal() + rhs.internal());
}

template <isQuantity Q> constexpr Q operator-(Q rhs) { return Q(-rhs.internal()); }

template <isQuantity Q, isQuantity R> constexpr Q operator-(Q lhs, R rhs)
    requires Isomorphic<Q, R>
{
    return Q(lhs.internal() - rhs.internal());
}

template <isQuantity Q>
    requires(!std::is_same_v<Q, Number>)
constexpr Q operator*(Q quantity, Number multiple) {
    return Q(quantity.internal() * multiple.internal());
}

template <isQuantity Q>
    requires(!std::is_same_v<Q, Number>)
constexpr Q operator*(Number multiple, Q quantity) {
    return Q(quantity.internal() * multiple.internal());
}

template <isQuantity Q>
    requires(!std::is_same_v<Q, Number>)
constexpr Q operator/(Q quantity, Number divisor) {
    return Q(quantity.internal() / divisor.internal());
}

template <isQuantity Q>
    requires(!std::is_same_v<Q, Number>)
constexpr auto operator/(Number enumerator, Q divisor) {
    return Named<Divided<Number, Q>>(enumerator.internal() / divisor.internal());
}

template <isQuantity Q1, isQuantity Q2>
constexpr std::conditional_t<std::is_same_v<Number, Multiplied<Q1, Q2>>, double, Multiplied<Q1, Q2>> operator*(Q1 lhs,
                                                                                                               Q2 rhs) {
    return std::conditional_t<std::is_same_v<Number, Multiplied<Q1, Q2>>, double, Multiplied<Q1, Q2>>(lhs.internal() *
                                                                                                      rhs.internal());
}

template <isQuantity Q1, isQuantity Q2>
constexpr std::conditional_t<std::is_same_v<Number, Multiplied<Q1, Q2>>, double, Divided<Q1, Q2>> operator/(Q1 lhs,
                                                                                                            Q2 rhs) {
    return std::conditional_t<std::is_same_v<Number, Multiplied<Q1, Q2>>, double, Divided<Q1, Q2>>(lhs.internal() /
                                                                                                   rhs.internal());
}

template <isQuantity Q, isQuantity R> constexpr bool operator==(const Q& lhs, const R& rhs)
    requires Isomorphic<Q, R>
{
    return (lhs.internal() == rhs.internal());
}

template <isQuantity Q, isQuantity R> constexpr bool operator!=(const Q& lhs, const R& rhs)
    requires Isomorphic<Q, R>
{
    return (lhs.internal() != rhs.internal());
}

template <isQuantity Q, isQuantity R> constexpr bool operator<=(const Q& lhs, const R& rhs)
    requires Isomorphic<Q, R>
{
    return (lhs.internal() <= rhs.internal());
}

template <isQuantity Q, isQuantity R> constexpr bool operator>=(const Q& lhs, const R& rhs)
    requires Isomorphic<Q, R>
{
    return (lhs.internal() >= rhs.internal());
}

template <isQuantity Q, isQuantity R> constexpr bool operator<(const Q& lhs, const R& rhs)
    requires Isomorphic<Q, R>
{
    return (lhs.internal() < rhs.internal());
}

template <isQuantity Q, isQuantity R> constexpr bool operator>(const Q& lhs, const R& rhs)
    requires Isomorphic<Q, R>
{
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
    template <> struct LookupName<Quantity<std::ratio<m>, std::ratio<l>, std::ratio<t>, std::ratio<i>, std::ratio<a>,  \
                                           std::ratio<o>, std::ratio<j>, std::ratio<n>>> {                             \
            using Named = Name;                                                                                        \
    };                                                                                                                 \
    [[maybe_unused]]                                                                                                   \
    constexpr Name suffix = Name(1.0);                                                                                 \
    constexpr Name operator""_##suffix(long double value) {                                                            \
        return Name(Quantity<std::ratio<m>, std::ratio<l>, std::ratio<t>, std::ratio<i>, std::ratio<a>, std::ratio<o>, \
                             std::ratio<j>, std::ratio<n>>(static_cast<double>(value)));                               \
    }                                                                                                                  \
    constexpr Name operator""_##suffix(unsigned long long value) {                                                     \
        return Name(Quantity<std::ratio<m>, std::ratio<l>, std::ratio<t>, std::ratio<i>, std::ratio<a>, std::ratio<o>, \
                             std::ratio<j>, std::ratio<n>>(static_cast<double>(value)));                               \
    }                                                                                                                  \
    template <> struct std::formatter<Name> : std::formatter<double> {                                                 \
            auto format(const Name& number, std::format_context& ctx) const {                                          \
                auto formatted_double = std::formatter<double>::format(number.internal(), ctx);                        \
                return std::format_to(formatted_double, "_" #suffix);                                                  \
            }                                                                                                          \
    };                                                                                                                 \
    inline std::ostream& operator<<(std::ostream& os, const Name& quantity) {                                          \
        os << quantity.internal() << " " << #suffix;                                                                   \
        return os;                                                                                                     \
    }                                                                                                                  \
    constexpr inline Name from_##suffix(double value) { return Name(value); }                                          \
    constexpr inline Name from_##suffix(Number value) { return Name(value.internal()); }                               \
    constexpr inline double to_##suffix(Name quantity) { return quantity.internal(); }

#define NEW_UNIT_LITERAL(Name, suffix, multiple)                                                                       \
    [[maybe_unused]]                                                                                                   \
    constexpr Name suffix = multiple;                                                                                  \
    constexpr Name operator""_##suffix(long double value) { return static_cast<double>(value) * multiple; }            \
    constexpr Name operator""_##suffix(unsigned long long value) { return static_cast<double>(value) * multiple; }     \
    constexpr inline Name from_##suffix(Number value) { return value.internal() * multiple; }                          \
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

template <> struct LookupName<Quantity<std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>, std::ratio<0>,
                                       std::ratio<0>, std::ratio<0>, std::ratio<0>>> {
        using Named = Number;
};

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
// Helper: if T is arithmetic, convert it to Number; otherwise leave it
// unchanged.
template <typename T> constexpr auto to_quantity(T value) -> std::conditional_t<std::is_arithmetic_v<T>, Number, T> {
    if constexpr (std::is_arithmetic_v<T>) return Number(value);
    else return value;
}

// abs: works with either a quantity or an arithmetic value.
template <typename T> constexpr auto abs(const T& lhs) {
    auto q = to_quantity(lhs);
    using Q = decltype(q);
    return Q(std::abs(q.internal()));
}

// max: converts both arguments; a static_assert ensures the resulting
// quantities are isomorphic.
template <typename T, typename U> constexpr auto max(const T& lhs, const U& rhs) {
    auto qlhs = to_quantity(lhs);
    auto qrhs = to_quantity(rhs);
    static_assert(Isomorphic<decltype(qlhs), decltype(qrhs)>, "max: Quantities must be isomorphic");
    return (qlhs > qrhs ? qlhs : qrhs);
}

// min: similar to max.
template <typename T, typename U> constexpr auto min(const T& lhs, const U& rhs) {
    auto qlhs = to_quantity(lhs);
    auto qrhs = to_quantity(rhs);
    static_assert(Isomorphic<decltype(qlhs), decltype(qrhs)>, "min: Quantities must be isomorphic");
    return (qlhs < qrhs ? qlhs : qrhs);
}

// sgn: always returns a Number.
template <typename T> constexpr auto sgn(const T& lhs) {
    auto q = to_quantity(lhs);
    if (q.internal() > 0) return Number(1);
    if (q.internal() < 0) return Number(-1);
    return Number(0);
}

// pow: now accepts an arithmetic or quantity value.
template <int R, typename T> constexpr auto pow(const T& lhs) {
    auto q = to_quantity(lhs);
    using Q = decltype(q);
    using S = Exponentiated<Q, std::ratio<R>>;
    return S(std::pow(q.internal(), R));
}

// square and cube call pow with 2 or 3.
template <typename T> constexpr auto square(const T& lhs) { return pow<2>(lhs); }

template <typename T> constexpr auto cube(const T& lhs) { return pow<3>(lhs); }

// root: similarly for roots.
template <int R, typename T> constexpr auto root(const T& lhs) {
    auto q = to_quantity(lhs);
    using Q = decltype(q);
    using S = Rooted<Q, std::ratio<R>>;
    return S(std::pow(q.internal(), 1.0 / R));
}

template <typename T> constexpr auto sqrt(const T& lhs) { return root<2>(lhs); }

template <typename T> constexpr auto cbrt(const T& lhs) { return root<3>(lhs); }

// hypot: requires the two quantities be isomorphic.
template <typename T, typename U> constexpr auto hypot(const T& lhs, const U& rhs) {
    auto qlhs = to_quantity(lhs);
    auto qrhs = to_quantity(rhs);
    static_assert(Isomorphic<decltype(qlhs), decltype(qrhs)>, "hypot: Quantities must be isomorphic");
    return decltype(qlhs)(std::hypot(qlhs.internal(), qrhs.internal()));
}

// mod: using std::fmod.
template <typename T, typename U> constexpr auto mod(const T& lhs, const U& rhs) {
    auto qlhs = to_quantity(lhs);
    auto qrhs = to_quantity(rhs);
    static_assert(Isomorphic<decltype(qlhs), decltype(qrhs)>, "mod: Quantities must be isomorphic");
    return decltype(qlhs)(std::fmod(qlhs.internal(), qrhs.internal()));
}

// remainder: using std::remainder.
template <typename T, typename U> constexpr auto remainder(const T& lhs, const U& rhs) {
    auto qlhs = to_quantity(lhs);
    auto qrhs = to_quantity(rhs);
    return decltype(qlhs)(std::remainder(qlhs.internal(), qrhs.internal()));
}

// copysign: applies std::copysign.
template <typename T, typename U> constexpr auto copysign(const T& lhs, const U& rhs) {
    auto qlhs = to_quantity(lhs);
    auto qrhs = to_quantity(rhs);
    return decltype(qlhs)(std::copysign(qlhs.internal(), qrhs.internal()));
}

// signbit: returns a bool.
template <typename T> constexpr auto signbit(const T& lhs) {
    auto q = to_quantity(lhs);
    return std::signbit(q.internal());
}

// clamp: requires three isomorphic quantities.
template <typename T, typename U, typename V> constexpr auto clamp(const T& lhs, const U& lo, const V& hi) {
    auto qlhs = to_quantity(lhs);
    auto qlo = to_quantity(lo);
    auto qhi = to_quantity(hi);
    static_assert(Isomorphic<decltype(qlhs), decltype(qlo), decltype(qhi)>, "clamp: Quantities must be isomorphic");
    return decltype(qlhs)(std::clamp(qlhs.internal(), qlo.internal(), qhi.internal()));
}

// ceil: rounds up to a multiple of rhs.
template <typename T, typename U> constexpr auto ceil(const T& lhs, const U& rhs) {
    auto qlhs = to_quantity(lhs);
    auto qrhs = to_quantity(rhs);
    static_assert(Isomorphic<decltype(qlhs), decltype(qrhs)>, "ceil: Quantities must be isomorphic");
    return decltype(qlhs)(std::ceil(qlhs.internal() / qrhs.internal()) * qrhs.internal());
}

// floor: rounds down.
template <typename T, typename U> constexpr auto floor(const T& lhs, const U& rhs) {
    auto qlhs = to_quantity(lhs);
    auto qrhs = to_quantity(rhs);
    static_assert(Isomorphic<decltype(qlhs), decltype(qrhs)>, "floor: Quantities must be isomorphic");
    return decltype(qlhs)(std::floor(qlhs.internal() / qrhs.internal()) * qrhs.internal());
}

// trunc: rounds toward zero.
template <typename T, typename U> constexpr auto trunc(const T& lhs, const U& rhs) {
    auto qlhs = to_quantity(lhs);
    auto qrhs = to_quantity(rhs);
    static_assert(Isomorphic<decltype(qlhs), decltype(qrhs)>, "trunc: Quantities must be isomorphic");
    return decltype(qlhs)(std::trunc(qlhs.internal() / qrhs.internal()) * qrhs.internal());
}

// round: rounds to the nearest.
template <typename T, typename U> constexpr auto round(const T& lhs, const U& rhs) {
    auto qlhs = to_quantity(lhs);
    auto qrhs = to_quantity(rhs);
    static_assert(Isomorphic<decltype(qlhs), decltype(qrhs)>, "round: Quantities must be isomorphic");
    return decltype(qlhs)(std::round(qlhs.internal() / qrhs.internal()) * qrhs.internal());
}
} // namespace units

// Convert an angular unit `Q` to a linear unit correctly;
// mostly useful for velocities
template <isQuantity Q>
constexpr Quantity<typename Q::mass, typename Q::angle, typename Q::time, typename Q::current, typename Q::length,
                   typename Q::temperature, typename Q::luminosity, typename Q::moles>
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
template <isQuantity Q>
constexpr Quantity<typename Q::mass, typename Q::angle, typename Q::time, typename Q::current, typename Q::length,
                   typename Q::temperature, typename Q::luminosity, typename Q::moles>
toAngular(Quantity<typename Q::mass, typename Q::length, typename Q::time, typename Q::current, typename Q::angle,
                   typename Q::temperature, typename Q::luminosity, typename Q::moles>
              linear,
          Length diameter) {
    return unit_cast<Quantity<typename Q::mass, typename Q::angle, typename Q::time, typename Q::current,
                              typename Q::length, typename Q::temperature, typename Q::luminosity, typename Q::moles>>(
        linear / (diameter / 2.0));
}
