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
    protected:
        T x; /** x component */
        T y; /** y component */
    public:
        /**
         * @brief Construct a new Vector2D object
         *
         * This constructor initializes x and y to 0
         *
         */
        Vector2D() : x(0.0), y(0.0) {}

        /**
         * @brief Construct a new Vector2D object
         *
         * This constructor initializes x and y to the given values
         *
         * @param nx x component
         * @param ny y component
         */
        Vector2D(T nx, T ny) : x(nx), y(ny) {}

        /**
         * @brief Create a new Vector2D object from polar coordinates
         *
         * This constructor takes polar coordinates and converts them to cartesian coordinates
         *
         * @param t angle
         * @param m magnitude
         */
        static Vector2D fromPolar(Angle t, T m) {
            m = m.abs();
            t = constrainAngle360(t);
            return Vector2D<T>(m * cos(t), m * sin(t));
        }

        /**
         * @brief Create a new Vector2D object from an angle with a magnitude of 1
         *
         * @param t angle
         * @return Vector2D
         */
        static Vector2D unitVector(Angle t) { return fromPolar(t, (T)1.0); }

        /**
         * @brief get the x component
         *
         * @return T x component
         */
        T getX() { return x; }

        /**
         * @brief get the y component
         *
         * @return T y component
         */
        T getY() { return y; }

        /**
         * @brief set the x component
         *
         * @param nx x component
         */
        void setX(T nx) { x = nx; }

        /**
         * @brief set the y component
         *
         * @param ny y component
         */
        void setY(T ny) { y = ny; }

        /**
         * @brief + operator overload
         *
         * This operator adds the x and y components of two vectors
         * {a, b} + {c, d} = {a + c, b + d}
         *
         * @param other vector to add
         * @return Vector2D<T>
         */
        Vector2D<T> operator+(Vector2D<T>& other) { return Vector2D<T>(x + other.getX(), y + other.getY()); }

        /**
         * @brief - operator overload
         *
         * This operator subtracts the x and y components of two vectors
         * {a, b} - {c, d} = {a - c, b - d}
         *
         * @param other vector to subtract
         * @return Vector2D<T>
         */
        Vector2D<T> operator-(Vector2D<T>& other) { return Vector2D<T>(x - other.getX(), y - other.getY()); }

        /**
         * @brief * operator overload
         *
         * This operator multiplies the x and y components of a vector by a scalar
         * a * {b, c} = {a * b, a * c}
         *
         * @param factor scalar to multiply by
         * @return Vector2D<T>
         */
        Vector2D<T> operator*(double factor) { return Vector2D<T>(x * factor, y * factor); }

        /**
         * @brief / operator overload
         *
         * This operator divides the x and y components of a vector by a scalar
         * {a, b} / c = {a / c, b / c}
         *
         * @param factor scalar to divide by
         * @return Vector2D<T>
         */
        Vector2D<T> operator/(double factor) { return Vector2D<T>(x / factor, y / factor); }

        /**
         * @brief += operator overload
         *
         * This operator adds the x and y components of two vectors and stores the result in the calling vector
         * {a, b} += {c, d} => {a + c, b + d}
         *
         * @param other vector to add
         * @return Vector2D<T>&
         */
        Vector2D<T>& operator+=(Vector2D<T>& other) {
            x += other.getX();
            y += other.getY();
            return (*this);
        }

        /**
         * @brief -= operator overload
         *
         * This operator subtracts the x and y components of two vectors and stores the result in the calling vector
         * {a, b} -= {c, d} => {a - c, b - d}
         *
         * @param other vector to subtract
         * @return Vector2D<T>&
         */
        Vector2D<T>& operator-=(Vector2D<T>& other) {
            x -= other.getX();
            y -= other.getY();
            return (*this);
        }

        /**
         * @brief *= operator overload
         *
         * This operator multiplies the x and y components of a vector by a scalar and stores the result in the
         * calling vector
         * a *= {b, c} => {a * b, a * c}
         *
         * @param factor scalar to multiply by
         * @return Vector2D<T>&
         */
        Vector2D<T>& operator*=(double factor) {
            x *= factor;
            y *= factor;
            return (*this);
        }

        /**
         * @brief /= operator overload
         *
         * This operator divides the x and y components of a vector by a scalar and stores the result in the
         * calling vector
         * {a, b} /= c => {a / c, b / c}
         *
         * @param factor scalar to divide by
         * @return Vector2D<T>&
         */
        Vector2D<T>& operator/=(double factor) {
            x /= factor;
            y /= factor;
            return (*this);
        }

        /**
         * @brief dot product of 2 Vector2D objects
         *
         * This function calculates the dot product of two vectors
         * a.dot(b) = (a.x * b.x) + (a.y * b.y)
         *
         * @tparam Q the type of quantity to use for the other vector
         * @tparam R the type of quantity to use for the result
         * @param other the vector to calculate the dot product with
         * @return R the dot product
         */
        template <isQuantity Q, isQuantity R = Multiplied<T, Q>> R dot(Vector2D<Q>& other) {
            return (x * other.getX()) + (y * other.getY());
        }

        /**
         * @brief cross product of 2 Vector2D objects
         *
         * This function calculates the cross product of two vectors
         * a.cross(b) = (a.x * b.y) - (a.y * b.x)
         *
         * @tparam Q the type of quantity to use for the other vector
         * @tparam R the type of quantity to use for the result
         * @param other the vector to calculate the cross product with
         * @return R the cross product
         */
        template <isQuantity Q, isQuantity R = Multiplied<T, Q>> R cross(Vector2D<Q>& other) {
            return (x * other.getY()) - (y * other.getX());
        }

        /**
         * @brief angle of the vector
         *
         * @return Angle
         */
        Angle theta() { return atan2(y, x); }

        /**
         * @brief magnitude of the vector
         *
         * @return T
         */
        T magnitude() { return sqrt(square(x) + square(y)); }

        /**
         * @brief difference between two vectors
         *
         * This function calculates the difference between two vectors
         * a.vectorTo(b) = {b.x - a.x, b.y - a.y}
         *
         * @param other the other vector
         * @return Vector2D<T>
         */
        Vector2D<T> vectorTo(Vector2D<T>& other) { return Vector2D<T>(other.getX() - x, other.getY() - y); }

        /**
         * @brief the angle between two vectors
         *
         * @param other the other vector
         * @return Angle
         */
        Angle angleTo(Vector2D<T>& other) { return atan2(other.getY() - y, other.getX() - x); }

        /**
         * @brief get the distance between two vectors
         *
         * @param other the other vector
         * @return T
         */
        T distanceTo(Vector2D<T>& other) { return sqrt(square(x - other.getX(), 2) + square(y - other.getY(), 2)); }

        /**
         * @brief normalize the vector
         *
         * This function normalizes the vector, making it a unit vector
         *
         * @return Vector2D<T>
         */
        Vector2D<T> normalize() {
            T m = magnitude();
            return Vector2D<T>(x / m, y / m);
        }

        /**
         * @brief rotate the vector by an angle
         *
         * @param angle
         */
        void rotateBy(Angle angle) {
            T m = magnitude();
            Angle t = theta() + angle;
            x = m * cos(t);
            y = m * sin(t);
        }

        /**
         * @brief rotate the vector to an angle
         *
         * @param angle
         */
        void rotateTo(Angle angle) {
            T m = magnitude();
            x = m * cos(angle);
            y = m * sin(angle);
        }

        /**
         * @brief get a copy of this vector rotated by an angle
         *
         * @param angle
         * @return Vector2D<T>
         */
        Vector2D<T> rotatedBy(Angle angle) {
            T m = magnitude();
            Angle t = theta() + angle;
            return fromPolar(t, m);
        }

        /**
         * @brief get a copy of this vector rotated to an angle
         *
         * @param angle
         * @return Vector2D<T>
         */
        Vector2D<T> rotatedTo(Angle angle) {
            T m = magnitude();
            return fromPolar(angle, m);
        }
};

// define some common vector types
typedef Vector2D<Length> V2Position;
typedef Vector2D<LinearVelocity> V2Velocity;
typedef Vector2D<LinearAcceleration> V2Acceleration;
typedef Vector2D<Force> V2Force;
} // namespace units