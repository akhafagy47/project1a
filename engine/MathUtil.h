#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <cmath>
#include <iostream>

namespace CMPUT350 {

struct Point2D {
    float x, y;
    Point2D(float x = 0, float y = 0) : x(x), y(y) {}
    double Distance(const Point2D &other) const {
        // TODO: write this code
        return std::sqrt((x-other.x)*(x-other.x) + (y-other.y)*(y-other.y));
    }
    Point2D operator+(const Point2D &other) const {
        // TODO: write this code
        return Point2D(x + other.x, y + other.y);
    }
    Point2D operator+(const float &other) const {
        // TODO: write this code
        return Point2D(x+other, y + other);
    }
    Point2D operator-(const Point2D &other) const {
        // TODO: write this code
        return Point2D(x - other.x, y - other.y);
    }
    Point2D operator-(const float &other) const {
        // TODO: write this code
        return Point2D(x - other, y - other);
    }
    Point2D operator*(const float &scalar) const {
        // TODO: write this code
        return Point2D(x*scalar, y*scalar);
    }
    Point2D &operator+=(const float &scalar) {
        // TODO: write this code
        x += scalar;
        y += scalar;
        return *this;
    }
    Point2D &operator+=(const Point2D &other) {
        // TODO: write this code
        x += other.x;
        y += other.y;
        return *this;
    }
    Point2D &operator-=(const Point2D &other) {
        // TODO: write this code
        x -= other.x;
        y -= other.y;
        return *this;
    }
    bool operator==(const Point2D &other) const {
        // TODO: write this code
        if (x==other.x && y==other.y)
            return true;
        return false;
    }
    Point2D &operator*=(const int &scalar) {
        // TODO: write this code
        x *= scalar;
        y *= scalar;
        return *this;
    }
    Point2D &operator/=(const int &scalar) {
        // TODO: write this code
        x /= scalar;
        y /= scalar;
        return *this;
    }
    float operator*(const Point2D &other) const {
        // TODO: write this code
        return x*other.x + y*other.y;
    }
    float Dot(Point2D b) const {
        // TODO: write this code
       return x*b.x + y*b.y;
    }
    static float Dot(Point2D a, Point2D b) {
        // TODO: write this code
        return a.x*b.x + a.y*b.y;
    }
    static float Cross(Point2D a, Point2D b) {
        // TODO: write this code
        return a.x*b.y - a.y*b.x;
    }
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

static std::ostream &operator<<(std::ostream &os, const Point2D &p) {
    // TODO: write this code
    os<<"("<<p.x<<" , "<<p.y<<")";
    return os;
}

static Point2D operator*(float number, const Point2D &rhs) {
    // TODO: write this code
    return rhs*number;
}

struct Line {
    Point2D p1, p2;

    Line(Point2D p1 = {0, 0}, Point2D p2 = {0, 0}) : p1(p1), p2(p2) {}
    Line(float x1, float y1, float x2, float y2) : p1(x1, y1), p2(x2, y2) {}
    float Length() const {
        // TODO: write this code
        return p1.Distance(p2);
    }
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

static std::ostream &operator<<(std::ostream &os, const Line &l) {
    // TODO: write this code
    os<<"["<<l.p1<<" -> "<<l.p2<<"]";
    return os;
}

struct Circle {
    Point2D center;
    float radius;

    Circle(Point2D c = {0, 0}, float r = 0) : center(c), radius(r) {}

    Circle(float x, float y, float r) : center(x, y), radius(r) {}
};

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
    Rect &operator|=(const Line &other) {
        // TODO: write this code
        *this |= other.p1;
        *this |= other.p2;
        return *this;
    }
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
    Rect &operator+=(const Point2D &other) {
        // TODO: write this code
        topLeft.x += other.x;
        topLeft.y += other.y;
        return *this;
    }
    Rect operator+(const Point2D &other) const {
        // TODO: write this code
        Rect interim_rect = *this;
        interim_rect += other;
        return interim_rect;
    }
    void Inset(int inset) {
        // TODO: write this code
        topLeft.x += inset;
        topLeft.y += inset;
        width -= (2 * inset);
        height -= (2 * inset);
    }
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

        if (cross_ab < 0.0f && cross_bc < 0.0f && cross_cd < 0.0f && cross_da < 0.0f)
        {
            return true;
        }
        else
        {
            return false;
        }
        return false;
    }
};

static std::ostream &operator<<(std::ostream &os, const Rect &l) {
    // TODO: write this code
    os<<"Rect[TopLeft: "<<l.topLeft<<", Width: "<<l.width<<", Height: "<<l.height<<"]";
    return os;
}

}  // namespace CMPUT350

#endif  // MATHUTIL_H
