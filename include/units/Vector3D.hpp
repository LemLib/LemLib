#pragma once

#include "units/Angle.hpp"

namespace units {
/**
 * @class Vector3D
 *
 * @brief a 3D vector with x, y, and z components of a given quantity type.
 *
 * @tparam T the type of quantity to use for the vector components
 */
template <isQuantity T> class Vector3D {
    public:
        T x; /** x component */
        T y; /** y component */
        T z; /** z component */

        /**
         * @brief Construct a new Vector2D object
         *
         * This constructor initializes x, y, and z to 0
         */
        constexpr Vector3D() : x(0.0), y(0.0), z(0.0) {}

        /**
         * @brief Construct a new Vector2D object
         *
         * This constructor initializes x, y, and z to the given values
         *
         * @param nx x component
         * @param ny y component
         * @param nz z component
         */
        constexpr Vector3D(T nx, T ny, T nz) : x(nx), y(ny), z(nz) {}

        /**
         * @brief Create a new Vector3D object from spherical coordinates
         *
         * This constructor takes polar coordinates and converts them to cartesian coordinates
         *
         * @param t angle
         * @param m magnitude
         */
        constexpr static Vector3D fromPolar(const Vector3D<Angle>& t, T m) {
            m = m.abs();
            return Vector3D<T>(m * cos(t.x), m * cos(t.y), m * cos(t.z));
        }

        /**
         * @brief Create a new Vector3D object from an angle with a magnitude of 1
         *
         * @param t angle
         * @return Vector3D
         */
        constexpr static Vector3D unitVector(const Vector3D<Angle>& t) { return fromPolar(t, (T)1.0); }

        /**
         * @brief + operator overload. Adds the components of two vectors
         *
         * {a, b, c} + {d, e, f} = {a + d, b + e, c + f}
         *
         * @param other vector to add
         * @return Vector3D<T>
         */
        constexpr Vector3D<T> operator+(const Vector3D<T>& other) const {
            return Vector3D<T>(this->x + other.x, this->y + other.y, this->z + other.z);
        }

        /**
         * @brief - operator overload. Subtracts the components of two vectors
         *
         * {a, b, c} - {d, e, f} = {a - d, b - e, c - f}
         *
         * @param other vector to subtract
         * @return Vector3D<T>
         */
        constexpr Vector3D<T> operator-(const Vector3D<T>& other) const {
            return Vector3D<T>(this->x - other.x, this->y - other.y, this->z - other.z);
        }

        /**
         * @brief * operator overload. Multiplies a vector by a double
         *
         * {a, b} * c = {a * c, b * c}
         *
         * @param factor the double to multiple the vector by
         * @return Vector2D<T>
         */
        constexpr Vector3D<T> operator*(double factor) const {
            return Vector3D<T>(this->x * factor, this->y * factor, this->z * factor);
        }

        /**
         * @brief * operator overload. Multiplies a vector by a quantity
         *
         * {a, b, c} * d = {a * d, b * d, c * d}
         *
         * @tparam Q the type of quantity to multiply the vector with
         * @tparam R the quantity type of the resulting vector
         *
         * @param factor the quantity to multiple the vector by
         * @return Vector3D<R>
         */
        template <isQuantity Q, isQuantity R = Multiplied<T, Q>> constexpr Vector3D<R> operator*(Q factor) const {
            return Vector3D<R>(this->x * factor, this->y * factor, this->z * factor);
        }

        /**
         * @brief / operator overload. Multiplies a vector by a double
         *
         * {a, b, c} / d = {a / d, b / d, c / d}
         *
         * @param factor the double to multiple the vector by
         * @return Vector3D<T>
         */
        constexpr Vector3D<T> operator/(double factor) const {
            return Vector3D<T>(this->x / factor, this->y / factor, this->z / factor);
        }

        /**
         * @brief / operator overload. Multiplies a vector by a quantity
         *
         * {a, b, c} / d = {a / d, b / d, c / d}
         *
         * @tparam Q the type of quantity to multiply the vector with
         * @tparam R the quantity type of the resulting vector
         *
         * @param factor the quantity to multiple the vector by
         * @return Vector3D<R>
         */
        template <isQuantity Q, isQuantity R = Divided<T, Q>> constexpr Vector3D<R> operator/(Q factor) const {
            return Vector3D<R>(this->x / factor, this->y / factor, this->z / factor);
        }

        /**
         * @brief += operator overload. Adds the components of two vectors and stores the result
         *
         * {a, b, c} += {d, e, f} => {a + d, b + e, c + f}
         *
         * @param other vector to add
         * @return Vector3D<T>&
         */
        constexpr Vector3D<T>& operator+=(const Vector3D<T>& other) {
            this->x += other.x;
            this->y += other.y;
            this->z += other.z;
            return (*this);
        }

        /**
         * @brief -= operator overload. Subtracts the components of two vectors and stores the result
         *
         * {a, b, c} -= {d, e, f} => {a - d, b - e, c - f}
         *
         * @param other vector to subtract
         * @return Vector3D<T>&
         */
        constexpr Vector3D<T>& operator-=(const Vector3D<T>& other) {
            this->x -= other.x;
            this->y -= other.y;
            this->z -= other.z;
            return (*this);
        }

        /**
         * @brief *= operator overload. Multiplies the components of a vector by a scalar and stores the result.
         *
         * a *= {b, c, d} => {a * b, a * c, a * d}
         *
         * @param factor scalar to multiply by
         * @return Vector3D<T>&
         */
        constexpr Vector3D<T>& operator*=(double factor) {
            this->x *= factor;
            this->y *= factor;
            this->z *= factor;
            return (*this);
        }

        /**
         * @brief /= operator overload. Divides the components of a vector by a scalar and stores the result
         *
         * {a, b, c} /= d => {a / d, b / d, c / d}
         *
         * @param factor scalar to divide by
         * @return Vector3D<T>&
         */
        constexpr Vector3D<T>& operator/=(double factor) {
            this->x /= factor;
            this->y /= factor;
            this->z /= factor;
            return (*this);
        }

        /**
         * @brief dot product of 2 Vector3D objects
         *
         * This function calculates the dot product of two vectors
         * a.dot(b) = (a.x * b.x) + (a.y * b.y) + (a.z * b.z)
         *
         * @tparam Q the type of quantity to use for the other vector
         * @tparam R the type of quantity to use for the result
         * @param other the vector to calculate the dot product with
         * @return R the dot product
         */
        template <isQuantity Q, isQuantity R = Multiplied<T, Q>> constexpr R dot(const Vector3D<Q>& other) const {
            return (this->x * other.x) + (this->y * other.y) + (this->z * other.z);
        }

        /**
         * @brief cross product of 2 Vector3D objects
         *
         * This function calculates the cross product of two vectors
         * a.cross(b) = { a.y * b.z - a.z * b.y,
         *                a.z * b.x - a.x * b.z,
         *                a.x * b.y - a.y * b.x }
         *
         * @tparam Q the type of quantity to use for the other vector
         * @tparam R the type of quantity to use for the result
         * @param other the vector to calculate the cross product with
         * @return Vector3D<R> the cross product
         */
        template <isQuantity Q, isQuantity R = Multiplied<T, Q>>
        constexpr Vector3D<R> cross(const Vector3D<Q>& other) const {
            return Vector3D<R>(this->y * other.z - this->z * other.y, this->z * other.x - this->x * other.z,
                               this->x * other.y - this->y * other.x);
        }

        /**
         * @brief angle of the vector
         *
         * @return Angle
         */
        constexpr Vector3D<Angle> theta() const {
            const T mag = magnitude();
            return Vector3D<Angle>(acos(this->x / mag), acos(this->y / mag), acos(this->z / mag));
        }

        /**
         * @brief magnitude of the vector
         *
         * @return T
         */
        constexpr T magnitude() const { return sqrt(square(this->x) + square(this->y) + square(this->z)); }

        /**
         * @brief the angle between two vectors
         *
         * @param other the other vector
         * @return Angle
         */
        constexpr Angle angleTo(const Vector3D<T>& other) const {
            return acos(dot(other) / (magnitude() * other.magnitude()));
        }

        /**
         * @brief get the distance between two vectors
         *
         * @param other the other vector
         * @return T
         */
        constexpr T distanceTo(const Vector3D<T>& other) const { return vectorTo(other).magnitude(); }

        /**
         * @brief normalize the vector
         *
         * This function normalizes the vector, making it a unit vector
         *
         * @return Vector3D<T>
         */
        constexpr Vector3D<T> normalize() { return (*this) / magnitude(); }

        /**
         * @brief rotate the vector by an angle
         *
         * @param angle
         */
        constexpr void rotateBy(const Vector3D<Angle>& angle) {
            const T m = magnitude();
            const Vector3D<Angle> t = theta() + angle;
            this->x = m * cos(t.x);
            this->y = m * cos(t.y);
            this->z = m * cos(t.z);
        }

        /**
         * @brief rotate the vector to an angle
         *
         * @param angle
         */
        constexpr void rotateTo(const Vector3D<Angle>& angle) {
            const T m = magnitude();
            this->x = m * cos(angle.x);
            this->y = m * cos(angle.y);
            this->z = m * cos(angle.z);
        }

        /**
         * @brief get a copy of this vector rotated by an angle
         *
         * @param angle
         * @return Vector3D<T>
         */
        constexpr Vector3D<T> rotatedBy(const Vector3D<Angle>& angle) const {
            return fromPolar(theta() + angle, magnitude());
        }

        /**
         * @brief get a copy of this vector rotated to an angle
         *
         * @param angle
         * @return Vector3D<T>
         */
        constexpr Vector3D<T> rotatedTo(Angle angle) const { return fromPolar(angle, magnitude()); }
};

/**
 * @brief * operator overload. Multiplies a scalar and a vector
 *
 * a * {b, c, d} = {a * b, a * c, a * d}
 *
 * @tparam Q1 the quantity type of the scalar
 * @tparam Q2 the quantity type of the vector
 * @tparam Q3 the type of quantity to use for the result
 *
 * @param lhs the scalar on the left hand side
 * @param rhs the vector on the right hand side
 * @return Q3 the product
 */
template <isQuantity Q1, isQuantity Q2, isQuantity Q3 = Multiplied<Q1, Q2>>
constexpr Vector3D<Q3> operator*(Q1 lhs, const Vector3D<Q2>& rhs) {
    return rhs * lhs;
}

/**
 * @brief * operator overload. Multiplies a double and a vector
 *
 * a * {b, c, d} = {a * b, a * c, a * d}
 *
 * @tparam Q the quantity type of the vector
 *
 * @param lhs the scalar on the left hand side
 * @param rhs the vector on the right hand side
 * @return Vector3D<Q> the product
 */
template <isQuantity Q> constexpr Vector3D<Q> operator*(double lhs, const Vector3D<Q>& rhs) { return rhs * lhs; }

// define some common vector types
typedef Vector3D<Length> V3Position;
typedef Vector3D<LinearVelocity> V3Velocity;
typedef Vector3D<LinearAcceleration> V3Acceleration;
typedef Vector3D<Force> V3Force;
} // namespace units