#include "lemlib/shapesncoords.hpp"

namespace lemlib {

    // Coordinate 

    Coordinate::Coordinate(float xarg, float yarg) {
        x = xarg;
        y = yarg;
    }

    float Coordinate::getX() {
        return x;
    }

    float Coordinate::getY() {
        return y;
    }

    void Coordinate::setX(float xarg) {
        x = xarg;
    }

    void Coordinate::setY(float yarg) {
        y = yarg;
    }

    void Coordinate::setCoordinate(float xarg, float yarg) {
        x = xarg;
        y = yarg;
    }

    void Coordinate::setCoordinate(Coordinate coordArg) {
        x = coordArg.getX();
        y = coordArg.getY();
    }

    void Coordinate::addX(float xarg) {
        x += xarg;
    }

    void Coordinate::addY(float yarg) {
        y += yarg;
    }

    // Shape

    Shape::Shape() {
        // Do nothing
    }

    Shape::Shape(float xarg, float yarg, float thetaArg) {
        x = xarg;
        y = yarg;
        theta = thetaArg;
    }

    Shape::Shape(Coordinate coordArg, float thetaArg) {
        x = coordArg.getX();
        y = coordArg.getY();
        theta = thetaArg;

    }

    float Shape::getX() {
        return x;
    }

    float Shape::getY() {
        return x;
    }

    float Shape::getTheta() {
        return theta;
    }

    void Shape::setX(float xarg) {
        x = xarg;
    }

    void Shape::setY(float yarg) {
        y = yarg;
    }

    void Shape::setTheta(float thetaarg) {
        theta = thetaarg;
    }

    void Shape::setCenter(float xarg, float yarg) {
        x = xarg;
        y = yarg;
    }

    void Shape::setCenter(Coordinate coordArg) {
        x = coordArg.getX();
        y = coordArg.getY();
    }

    void Shape::setShape(Shape shapeArg) {
        x = shapeArg.getX();
        y = shapeArg.getY();
        theta = shapeArg.getTheta();

    }

    void Shape::addX(float xarg) {
        x += xarg;
    }

    void Shape::addY(float yarg) {
        y += yarg;
    }

    void Shape::addTheta(float thetaarg) {
        theta += thetaarg;
    }

    bool Shape::shapeEquals(Shape shapeArg) {
        return (x == shapeArg.getX() && y == shapeArg.getY() && theta == shapeArg.getTheta());
    }


    // Circle


    Circle::Circle(float xarg, float yarg, float radiusArg) : Shape(xarg, yarg, 0) {
        radius = radiusArg;
    }

    float Circle::getRadius() {
        return radius;
    }

    void Circle::setRadius(float radiusArg) {
        radius = radiusArg;
    }

    bool Circle::overlapsCircle(Circle circleArg) {

        // If the distance between the centers is less than the sum of the radii, then the circles overlap
        return (sqrt(pow(x - circleArg.getX(), 2) + pow(y - circleArg.getY(), 2)) <= radius + circleArg.getRadius());

    }

    

    





}