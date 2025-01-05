#pragma once

#include "units/Angle.hpp"

namespace units {
/**
 * @class Vector2D
 *
 * @brief a 2D vector with x and y components of a given quantity type.
 *
 * @tparam T the type of quantity to use for the vector components
 */
template <isQuantity T> class Vector2D {
    public:
        T x; /** x component */
        T y; /** y component */

        /**
         * @brief Construct a new Vector2D object
         *
         * This constructor initializes x and y to 0
         *
         */
        constexpr Vector2D() : x(0.0), y(0.0) {}

        /**
         * @brief Construct a new Vector2D object
         *
         * This constructor initializes x and y to the given values
         *
         * @param nx x component
         * @param ny y component
         */
        constexpr Vector2D(T nx, T ny) : x(nx), y(ny) {}

        /**
         * @brief Create a new Vector2D object from polar coordinates
         *
         * This constructor takes polar coordinates and converts them to cartesian coordinates
         *
         * @param t angle
         * @param m magnitude
         */
        constexpr static Vector2D fromPolar(Angle t, T m) {
            m = abs(m);
            t = constrainAngle360(t);
            return Vector2D<T>(m * cos(t), m * sin(t));
        }

        /**
         * @brief Create a new Vector2D object from an angle with a magnitude of 1
         *
         * @param t angle
         * @return Vector2D
         */
        constexpr static Vector2D unitVector(Angle t) { return fromPolar(t, (T)1.0); }

        /**
         * @brief + operator overload. Adds the x and y components of two vectors
         *
         * {a, b} + {c, d} = {a + c, b + d}
         *
         * @param other vector to add
         * @return Vector2D<T>
         */
        constexpr Vector2D<T> operator+(const Vector2D<T>& other) const {
            return Vector2D<T>(this->x + other.x, this->y + other.y);
        }

        /**
         * @brief - operator overload. Substracts the x and y components of two vectors
         *
         * {a, b} - {c, d} = {a - c, b - d}
         *
         * @param other vector to subtract
         * @return Vector2D<T>
         */
        constexpr Vector2D<T> operator-(const Vector2D<T>& other) const {
            return Vector2D<T>(this->x - other.x, this->y - other.y);
        }

        /**
         * @brief * operator overload. Multiplies a vector by a double
         *
         * {a, b} * c = {a * c, b * c}
         *
         * @param factor the double to multiple the vector by
         * @return Vector2D<T>
         */
        constexpr Vector2D<T> operator*(double factor) const { return Vector2D<T>(this->x * factor, this->y * factor); }

        /**
         * @brief * operator overload. Multiplies a vector by a quantity
         *
         * {a, b} * c = {a * c, b * c}
         *
         * @tparam Q the type of quantity to multiply the vector with
         * @tparam R the quantity type of the resulting vector
         *
         * @param factor the quantity to multiple the vector by
         * @return Vector2D<R>
         */
        template <isQuantity Q, isQuantity R = Multiplied<T, Q>> constexpr Vector2D<R> operator*(Q factor) const {
            return Vector2D<R>(this->x * factor, this->y * factor);
        }

        /**
         * @brief * operator overload. Finds the dot product of 2 Vector2D objects
         *
         * {a, b} * {c, d} = (a * c) + (b * d)
         *
         * @tparam Q the type of quantity to use for the other vector
         * @tparam R the type of quantity to use for the result
         * @param other the vector to calculate the dot product with
         * @return R the dot product
         */
        template <isQuantity Q, isQuantity R = Multiplied<T, Q>> constexpr R operator*(const Vector2D<Q>& other) const {
            return (this->x * other.x) + (this->y * other.y);
        }

        /**
         * @brief / operator overload. Divides a vector by a double
         *
         * {a, b} / c = {a / c, b / c}
         *
         * @param factor the double to multiple the vector by
         * @return Vector2D<T>
         */
        constexpr Vector2D<T> operator/(double factor) const { return Vector2D<T>(this->x / factor, this->y / factor); }

        /**
         * @brief / operator overload. Divides a vector by a quantity
         *
         * {a, b} / c = {a / c, b / c}
         *
         * @tparam Q the type of quantity to divide the vector with
         * @tparam R the quantity type of the resulting vector
         *
         * @param factor the quantity to divide the vector by
         * @return Vector2D<T>
         */
        template <isQuantity Q, isQuantity R = Divided<T, Q>> Vector2D<R> constexpr operator/(Q factor) const {
            return Vector2D<R>(this->x / factor, this->y / factor);
        }

        /**
         * @brief += operator overload. Adds the components of two vectors and stores the result
         *
         * {a, b} += {c, d} => {a + c, b + d}
         *
         * @param other vector to add
         * @return Vector2D<T>&
         */
        constexpr Vector2D<T>& operator+=(const Vector2D<T>& other) {
            this->x += other.x;
            this->y += other.y;
            return (*this);
        }

        /**
         * @brief -= operator overload. Subtracts the components of two vectors and stores the result
         *
         * {a, b} -= {c, d} => {a - c, b - d}
         *
         * @param other vector to subtract
         * @return Vector2D<T>&
         */
        constexpr Vector2D<T>& operator-=(const Vector2D<T>& other) {
            this->x -= other.x;
            this->y -= other.y;
            return (*this);
        }

        /**
         * @brief *= operator overload. Multiplies the components of a vector by a scalar and stores the result
         *
         * {a, b} *= c => {a * c, b * c}
         *
         * @param factor scalar to multiply by
         * @return Vector2D<T>&
         */
        constexpr Vector2D<T>& operator*=(double factor) {
            this->x *= factor;
            this->y *= factor;
            return (*this);
        }

        /**
         * @brief /= operator overload. Divides the components of a vector by a scalar and stores the result
         *
         * {a, b} /= c => {a / c, b / c}
         *
         * @param factor scalar to divide by
         * @return Vector2D<T>&
         */
        constexpr Vector2D<T>& operator/=(double factor) {
            this->x /= factor;
            this->y /= factor;
            return (*this);
        }

        /**
         * @brief magnitude of the vector
         *
         * @return T
         */
        constexpr T magnitude() const { return sqrt(square(this->x) + square(this->y)); }

        /**
         * @brief the angle between two vectors
         *
         * @param other the other vector
         * @return Angle
         */
        constexpr Angle angleTo(const Vector2D<T>& other) const { return atan2(other.y - this->y, other.x - this->x); }

        /**
         * @brief get the distance between two vectors
         *
         * @param other the other vector
         * @return T
         */
        constexpr T distanceTo(const Vector2D<T>& other) const {
            return sqrt(square(this->x - other.x) + square(this->y - other.y));
        }

        /**
         * @brief normalize the vector
         *
         * This function normalizes the vector, making it a unit vector
         *
         * @return Vector2D<T>
         */
        constexpr Vector2D<T> normalize() const { return (*this) / magnitude(); }

        /**
         * @brief get a copy of this vector rotated by an angle
         *
         * @param angle
         * @return Vector2D<T>
         */
        constexpr Vector2D<T> rotatedBy(Angle angle) const {
            return fromPolar(Vector2D<T>({T(0.0), T(0.0)}).angleTo(*this) + angle, magnitude());
        }
};

template <typename T> inline constexpr Vector2D<T> origin({T(0.0), T(0.0)});

/**
 * @brief * operator overload. Multiplies a quantity and a vector
 *
 * a * {b, c} = {a * b, a * c}
 *
 * @tparam Q1 the quantity type of the scalar
 * @tparam Q2 the quantity type of the vector
 * @tparam Q3 the type of quantity to use for the result
 *
 * @param lhs the scalar on the left hand side
 * @param rhs the vector on the right hand side
 * @return Vector2D<Q3> the product
 */
template <isQuantity Q1, isQuantity Q2, isQuantity Q3 = Multiplied<Q1, Q2>>
constexpr Vector2D<Q3> operator*(Q1 lhs, const Vector2D<Q2>& rhs) {
    return rhs * lhs;
}

/**
 * @brief * operator overload. Multiplies a double and a vector
 *
 * a * {b, c} = {a * b, a * c}
 *
 * @tparam Q the quantity type of the vector
 *
 * @param lhs the scalar on the left hand side
 * @param rhs the vector on the right hand side
 * @return Vector2D<Q> the product
 */
template <isQuantity Q> constexpr Vector2D<Q> operator*(double lhs, const Vector2D<Q>& rhs) { return rhs * lhs; }

// define some common vector types
typedef Vector2D<Length> V2Position;
typedef Vector2D<LinearVelocity> V2Velocity;
typedef Vector2D<LinearAcceleration> V2Acceleration;
typedef Vector2D<Force> V2Force;
} // namespace units
