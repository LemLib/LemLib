#include "math.h"

namespace lemlib {

class Coordinate {
private:

protected:

    float x;
    float y;

public:

    Coordinate(float xarg, float yarg);

    /**
     * @brief Get the X coordinate
     * 
     * @return float 
     */
    float getX();

    /**
     * @brief Get the Y coordinate
     * 
     * @return float 
     */
    float getY();

    /**
     * @brief Set the X coordinate
     * 
     * @param xarg 
     */
    void setX(float xarg);

    /**
     * @brief Set the Y coordinate
     * 
     * @param yarg 
     */
    void setY(float yarg);

    /**
     * @brief Set the Coordinate to a new X and Y value
     * 
     * @param xarg X Value
     * @param yarg Y Value
     */
    void setCoordinate(float xarg, float yarg);

    /**
     * @brief Set the Shape object to a new Shape object of the same type.
     * 
     * @param coordArg 
     */
    void setCoordinate(Coordinate coordArg);

    /**
     * @brief Add to the X coordinate
     * 
     * @param xarg 
     */
    void addX(float xarg);

    /**
     * @brief Add to the Y coordinate
     * 
     * @param yarg 
     */
    void addY(float yarg);

    /**
     * @brief Checks if one shape = another shape
     * 
     * @param coordArg Input any shape / coordinate. 
     * @return true 
     * @return false 
     */
    bool shapeEquals(Coordinate coordArg);

};


class Shape {
protected:

    float x;
    float y;
    float theta; // Rotation

    Shape();
    
public:

    Shape(float xarg, float yarg, float thetaArg);

    Shape(Coordinate coordArg, float thetaArg);

    /**
     * @brief Get the X coordinate
     * 
     * @return float 
     */
    float getX();

    /**
     * @brief Get the Y coordinate
     * 
     * @return float 
     */
    float getY();
    
    /**
     * @brief Get the Theta value
     * 
     * @return float 
     */
    float getTheta();

    /**
     * @brief Set the X coordinate
     * 
     * @param xarg 
     */
    void setX(float xarg);

    /**
     * @brief Set the Y coordinate
     * 
     * @param yarg 
     */
    void setY(float yarg);

    /**
     * @brief Set the Theta value
     * 
     * @param thetaarg 
     */
    void setTheta(float thetaarg);

    /**
     * @brief Set the Shape to a new X and Y value
     * 
     * @param xarg X Value
     * @param yarg Y Value
     */
    void setCenter(float xarg, float yarg);

    /**
     * @brief Set the Shape to a new X and Y value
     * 
     * @param xarg X Value
     * @param yarg Y Value
     */
    void setCenter(Coordinate coordArg);

    /**
     * @brief Set the Shape object to a new Shape object of the same type.
     * 
     * @param shapearg 
     */
    void setShape(Shape shapearg);

    /**
     * @brief Add to the X coordinate
     * 
     * @param xarg 
     */
    void addX(float xarg);

    /**
     * @brief Add to the Y coordinate
     * 
     * @param yarg 
     */
    void addY(float yarg);

    /**
     * @brief Add to the Theta value
     * 
     * @param thetaarg 
     */
    void addTheta(float thetaarg);

    /**
     * @brief Checks if this shape object = another shape
     * 
     * @param shapearg Input any shape. 
     * @return true 
     * @return false 
     */
    bool shapeEquals(Shape shapearg);

    /**
     * @brief Checks if one shape overlaps another shape
     * 
     * @param shapearg Input any shape.
     * @return true 
     * @return false 
     */
    bool shapeOverlaps(Shape shapearg);

};

class Polygon : public Shape {
private:

protected:

    float width = 0;
    float height = 0;

public:

};



class Circle : public Shape {
private:

    float radius = 0;

public:

    /**
     * @brief Construct a new Circle object
     * 
     * @param xarg The center of the circle on the X axis
     * @param yarg The center of the circle on the Y axis
     * @param radiusarg The radius of the circle
     */
    Circle(float xarg, float yarg, float radiusarg);

    /**
     * @brief Get the radius of the circle
     * 
     * @return float 
     */
    float getRadius();

    void setRadius(float radiusArg);

    bool overlapsCircle(Circle circleArg);


};

class Elipse : public Shape {
private:

protected:

    float A = 1;
    float B = 1;

public:

    /**
     * @brief Construct a new Elipse object
     * 
     * @param xarg The center of the elipse on the X axis
     * @param yarg The center of the elipse on the Y axis
     * @param Aarg The A value of the elipse
     * @param Barg The B value of the elipse
     */
    Elipse(float xarg, float yarg, float Aarg, float Barg);

    /**
     * @brief Get the A value of the elipse
     * 
     * @return float 
     */
    float getA();

    /**
     * @brief Get the B value of the elipse
     * 
     * @return float 
     */
    float getB();

    /**
     * @brief Set the A value of the elipse
     * 
     * @param Aarg 
     */
    void setA(float Aarg);

    /**
     * @brief Set the B value of the elipse
     * 
     * @param Barg 
     */
    void setB(float Brag);

    /**
     * @brief Set the Elipse object to a new Elipse object of the same type.
     * 
     * @param elipsearg 
     */
    void setElipse(Elipse elipsearg);

    /**
     * @brief Set the Elipse object to a new Elipse object of the same type.
     * 
     * @param xarg The center of the elipse on the X axis
     * @param yarg The center of the elipse on the Y axis
     * @param Aarg The A value of the elipse
     * @param Barg The B value of the elipse
     */
    void setElipse(float xarg, float yarg, float Aarg, float Barg);

    /**
     * @brief Add to the A value of the elipse
     * 
     * @param Aarg 
     */
    void addA(float Aarg);

};

class Rectangle : public Polygon {
private:

    float width = 0;
    float height = 0;

protected:

public:

    

};

class Square : public Rectangle {
private:

    float sideLength = 0;

protected:

public:

    Square(float xarg, float yarg, float sideLengtharg);

    /**
     * @brief Get the Side Length of the square
     * 
     * @return float 
     */
    float getSideLength();

    /**
     * @brief Set the Side Length of the square
     * 
     * @param sideLengtharg 
     */
    void setSideLength(float sideLengtharg);

    /**
     * @brief Set the Square object to a new Square object of the same type.
     * 
     * @param squarearg 
     */
    void setSquare(Square squarearg);

    /**
     * @brief Set the Square object to a new Square object of the same type.
     * 
     * @param xarg 
     * @param yarg 
     * @param sideLengtharg 
     */
    void setSquare(float xarg, float yarg, float sideLengtharg);

    /**
     * @brief Add to the Side Length of the square
     * 
     * @param sideLengtharg 
     */
    void addSideLength(float sideLengtharg);

};

class Triangle : public Polygon {

};

}