#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <cmath>
#include <iostream>

namespace CMPUT350 {

/**
 * @struct Point2D
 * @brief Represents a 2D coordinate or vector of a point.
 */
struct Point2D {
    float x, y;

    /// @brief Constructs a Point2D with x and y coordinates.
    Point2D(float x = 0, float y = 0) : x(x), y(y) {}

    /// @brief Calculates the Euclidean distance between the point in the struct to another Point2D.
    double Distance(const Point2D &other) const {
        // TODO: write this code
        return std::sqrt((x-other.x)*(x-other.x) + (y-other.y)*(y-other.y));
    }

    /// @brief Adds the coordinates of the point in the struct and another Point2D together.
    Point2D operator+(const Point2D &other) const {
        // TODO: write this code
        return Point2D(x + other.x, y + other.y);
    }

    /// @brief Adds a scalar value to both coordinates of the point in the struct.
    Point2D operator+(const float &other) const {
        // TODO: write this code
        return Point2D(x+other, y + other);
    }

    /// @brief Subtracts another Point2D from the one in the struct.
    Point2D operator-(const Point2D &other) const {
        // TODO: write this code
        return Point2D(x - other.x, y - other.y);
    }

    /// @brief Subtracts a scalar value from both coordinates of the point in the struct.
    Point2D operator-(const float &other) const {
        // TODO: write this code
        return Point2D(x - other, y - other);
    }

    /// @brief Multiplies both coordinates of the point in the struct by a scalar value.
    Point2D operator*(const float &scalar) const {
        // TODO: write this code
        return Point2D(x*scalar, y*scalar);
    }

    /// @brief Adds a scalar to both coordinates of the point in the struct in place.
    Point2D &operator+=(const float &scalar) {
        // TODO: write this code
        x += scalar;
        y += scalar;
        return *this;
    }

    /// @brief Adds another Point2D to the one in the struct in place.
    Point2D &operator+=(const Point2D &other) {
        // TODO: write this code
        x += other.x;
        y += other.y;
        return *this;
    }

    /// @brief Subtracts a scalar from both coordinates of the point in the struct in place.
    Point2D &operator-=(const Point2D &other) {
        // TODO: write this code
        x -= other.x;
        y -= other.y;
        return *this;
    }

    /// @brief Checks if the point in the struct has the same coordinates as another Point2D.
    bool operator==(const Point2D &other) const {
        // TODO: write this code
        if (x==other.x && y==other.y)
            return true;
        return false;
    }

    /// @brief Multiplies both coordinates of the point in the struct by a scalar in place.
    Point2D &operator*=(const int &scalar) {
        // TODO: write this code
        x *= scalar;
        y *= scalar;
        return *this;
    }

    /// @brief Divides both coordinates of the point in the struct by a scalar in place.
    Point2D &operator/=(const int &scalar) {
        // TODO: write this code
        x /= scalar;
        y /= scalar;
        return *this;
    }

    /// @brief Overloads multiplication operator to calculate the dot product  of the point in the struct with another point.
    float operator*(const Point2D &other) const {
        // TODO: write this code
        return x*other.x + y*other.y;
    }
    /// @brief Calculates the dot product of the point in the struct with another point.
    float Dot(Point2D b) const {
        // TODO: write this code
       return x*b.x + y*b.y;
    }

    /// @brief Static function that calculates the dot product of two Point2D.
    static float Dot(Point2D a, Point2D b) {
        // TODO: write this code
        return a.x*b.x + a.y*b.y;
    }

    /// @brief Static function that calculates the cross product magnitude of two Point2D.
    static float Cross(Point2D a, Point2D b) {
        // TODO: write this code
        return a.x*b.y - a.y*b.x;
    }
    /// @brief Normalizes the vecor of the point to a magnitude of 1.
    void Normalize() {
        // TODO: write this code
        float magnitude = std::sqrt(x*x + y*y);
        if (magnitude>0)
        {
            x /= magnitude;
            y /= magnitude;
        }                
           
    }
};

/// @brief Puts a formatted Point2D into the output stream.
static std::ostream &operator<<(std::ostream &os, const Point2D &p) {
    // TODO: write this code
    os<<"("<<p.x<<" , "<<p.y<<")";
    return os;
}


/// @brief Allows scalar multiplication where the scalar is the left-hand operand.
static Point2D operator*(float number, const Point2D &rhs) {
    // TODO: write this code
    return rhs*number;
}

/**
 * @struct Line
 * @brief Represents a 2D line segment defined by two points.
 */
struct Line {
    Point2D p1, p2;

    /// @brief Constructs a line segment using two Point2D objects.
    Line(Point2D p1 = {0, 0}, Point2D p2 = {0, 0}) : p1(p1), p2(p2) {}

    /// @brief Constructs a line segment using raw coordinate values.
    Line(float x1, float y1, float x2, float y2) : p1(x1, y1), p2(x2, y2) {}
    
    /// @brief Calculates the length of the line segment.
    float Length() const {
        // TODO: write this code
        return p1.Distance(p2);
    }


    /**
     * @brief Calculates the closest point on the line segment in the struct to the given point.
     * 
     * Uses vector projection to find the closest coordinate along the infinite line. 
     * Clamps the calculated scalar between 0.0 and 1.0 tomake sure the calculated point
     * lies within the line segment bounds defined by p1 and p2.
     * 
     * @param p The target Point2D coordinate.
     * @return The closest Point2D that lies on the line segment.
     */
    Point2D ClosestPoint(const Point2D &p) const {
        // TODO: write this code
        Point2D original_line_vector = p2-p1;
        Point2D p1_to_p_vector = p-p1;
        if (!(p1==p2))
        {
            float scale = Point2D::Dot(original_line_vector, p1_to_p_vector)/Point2D::Dot(original_line_vector, original_line_vector);
            if (scale < 0.0f)
            {
                scale = 0.0f;
            }

            if (scale > 1.0f)
            {
                scale = 1.0f;
            }
            return p1 + scale*original_line_vector;
        }
        else
        {
            return p1;
        }
        
    }
        /**
     * @brief Checks if the line in the struct intersects with another line segment.
     * 
     * Uses 2D cross products to find the intersection point of two lines. 
     * It calculates scalar values (this_scale and other_scale) for both lines. 
     * If the scalars are between 0.0 and 1.0, the intersection occurs within 
     * the bounds of both line segments.
     * 
     * @param other The second Line to check for intersection.
     * @param crossingPoint Reference to a Point2D which stores intersection coordinate if they cross.
     * @return true if the line segments intersect, false if they don't.
     */
    bool Crosses(Line other, Point2D &crossingPoint) const {
        // TODO: write this code
       Point2D this_line_vector = p2 - p1;
        Point2D other_line_vector = other.p2 - other.p1;
        Point2D start_difference_vector = other.p1 - p1;
        
        float denominator = Point2D::Cross(this_line_vector, other_line_vector);
        
        if (denominator != 0.0f)
        {
            float this_scale = Point2D::Cross(start_difference_vector, other_line_vector) / denominator;
            float other_scale = Point2D::Cross(start_difference_vector, this_line_vector) / denominator;
            
            if (this_scale >= 0.0f && this_scale <= 1.0f && other_scale >= 0.0f && other_scale <= 1.0f)
            {
                crossingPoint = p1 + this_scale * this_line_vector;
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
};

/// @brief Inserts a formatted Line into the output stream.
static std::ostream &operator<<(std::ostream &os, const Line &l) {
    // TODO: write this code
    os<<"["<<l.p1<<" -> "<<l.p2<<"]";
    return os;
}

/**
 * @struct Circle
 * @brief Represents a circle defined by a center point and radius.
 */
struct Circle {
    Point2D center;
    float radius;

    Circle(Point2D c = {0, 0}, float r = 0) : center(c), radius(r) {}

    Circle(float x, float y, float r) : center(x, y), radius(r) {}
};

/**
 * @struct Rect
 * @brief Represents an axis-aligned 2D bounding box.
 */
struct Rect {
    Point2D topLeft;
    float width, height;

    Rect(float left, float top, float width, float height)
        : topLeft(Point2D(left, top)), width(width), height(height) {}

    Rect(Point2D tl = {0, 0}, int w = 0, int h = 0) : topLeft(tl), width(w), height(h) {}

    // Creates bounding box around p1 and p2 with positive width/height
    Rect(Point2D p1, Point2D p2)
        : topLeft(std::min(p1.x, p2.x), std::min(p1.y, p2.y)),
          width(fabs(p1.x - p2.x)),
          height(fabs(p1.y - p2.y)) {}

    Rect(Point2D center, float radius)
        : topLeft(center.x - radius, center.y - radius), width(2 * radius), height(2 * radius) {}

    /**
     * @brief Expands this rectangle to fully contain another rectangle.
     * @param other The rectangle to union with.
     * @return A reference to the modified rectangle.
     */
    Rect &operator|=(const Rect &other) {
        // TODO: write this code
        float current_right = topLeft.x + width;
        float current_bottom = topLeft.y + height;
        
        float other_right = other.topLeft.x + other.width;
        float other_bottom = other.topLeft.y + other.height;

        topLeft.x = std::min(topLeft.x, other.topLeft.x);
        topLeft.y = std::min(topLeft.y, other.topLeft.y);
        width = std::max(current_right, other_right) - std::min(topLeft.x, other.topLeft.x);
        height = std::max(current_bottom, other_bottom) - std::min(topLeft.y, other.topLeft.y);

        return *this;
    }

    /**
     * @brief Expands this rectangle to contain a given point.
     * @param other The Point2D to include in the bounding box.
     * @return A reference to the modified rectangle.
     */
    Rect &operator|=(const Point2D &other) {
        // TODO: write this code
        float current_right = topLeft.x + width;
        float current_bottom = topLeft.y + height;

        topLeft.x =std::min(topLeft.x, other.x);
        topLeft.y = std::min(topLeft.y, other.y);
        width = std::max(current_right, other.x) - std::min(topLeft.x, other.x);
        height = std::max(current_bottom, other.y) - std::min(topLeft.y, other.y);

        return *this;
    }

    /**
     * @brief Expands this rectangle to contain an entire line segment.
     * @param other The Line to include in the bounding box.
     * @return A reference to the modified rectangle.
     */
    Rect &operator|=(const Line &other) {
        // TODO: write this code
        *this |= other.p1;
        *this |= other.p2;
        return *this;
    }

    /**
     * @brief Modifies the rectangle in the struct to be the overlapping intersection with another.
     * 
     * If the rectangles don't overlap, the width and height are set to 0.0f.
     * 
     * @param other The rectangle to intersect with.
     * @return A reference to the modified rectangle.
     */
    Rect &operator&=(const Rect &other) {
        // TODO: write this code
        float current_right = topLeft.x + width;
        float current_bottom = topLeft.y + height;
        
        float other_right = other.topLeft.x + other.width;
        float other_bottom = other.topLeft.y + other.height;

        float overlap_left = std::max(topLeft.x, other.topLeft.x);
        float overlap_top = std::max(topLeft.y, other.topLeft.y);
        float overlap_right = std::min(current_right, other_right);
        float overlap_bottom = std::min(current_bottom, other_bottom);

        if (overlap_left < overlap_right && overlap_top < overlap_bottom)
        {
            topLeft.x = overlap_left;
            topLeft.y = overlap_top;
            width = overlap_right - overlap_left;
            height = overlap_bottom - overlap_top;
        }
        else
        {
            width = 0.0f;
            height = 0.0f;
        }

        return *this;
    }

    /// @brief Shifts the rectangle in the struct by adding a Point2D offset to its top-left corner in place.
    Rect &operator+=(const Point2D &other) {
        // TODO: write this code
        topLeft.x += other.x;
        topLeft.y += other.y;
        return *this;
    }

    /// @brief Returns a new rectangle shifted by a Point2D offset.
    Rect operator+(const Point2D &other) const {
        // TODO: write this code
        Rect interim_rect = *this;
        interim_rect += other;
        return interim_rect;
    }

    /// @brief Shrinks the rectangle in the struct's boundaries inward by the specified amount on all sides.
    void Inset(int inset) {
        // TODO: write this code
        topLeft.x += inset;
        topLeft.y += inset;
        width -= (2 * inset);
        height -= (2 * inset);
    }
    /**
     * @brief Checks if a 2D point is inside the rectangle in the struct.
     * 
     * Defines the four vertices of the rectangle and calculates the cross product 
     * between each edge vector and the vector from the vertex to the 
     * target point. If all cross products are positive, the point is inside the bounds.
     * 
     * @param p The Point2D coordinate to check.
     * @return true if the point is inside or on the edge of the rectangle, false if not.
     */
    bool IsInside(const Point2D &p) const {
        // TODO: write this code
        Point2D corner_a = topLeft;
        Point2D corner_b = Point2D(topLeft.x + width, topLeft.y);
        Point2D corner_c = Point2D(topLeft.x + width, topLeft.y + height);
        Point2D corner_d = Point2D(topLeft.x, topLeft.y + height);


        float cross_ab = Point2D::Cross(corner_b - corner_a, p - corner_a);

        float cross_bc = Point2D::Cross(corner_c - corner_b, p - corner_b);

        float cross_cd = Point2D::Cross(corner_d - corner_c, p - corner_c);

        float cross_da = Point2D::Cross(corner_a - corner_d, p - corner_d);

        if (cross_ab >= 0.0f && cross_bc >= 0.0f && cross_cd >= 0.0f && cross_da >= 0.0f)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};

/// @brief Inserts a formatted Rect into the output stream.
static std::ostream &operator<<(std::ostream &os, const Rect &l) {
    // TODO: write this code
    os<<"Rect[TopLeft: "<<l.topLeft<<", Width: "<<l.width<<", Height: "<<l.height<<"]";
    return os;
}

}  // namespace CMPUT350

#endif  // MATHUTIL_H
