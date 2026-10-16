#ifndef BEZIER_H
#define BEZIER_H

#include <vector>

#include "MathUtil.h"

namespace CMPUT350 {

class Bezier {
public:
    Bezier(const std::vector<Point2D>& pts);
    Point2D GetPoint(float t) const;
    Vector2D GetSlope(float t) const;
    static Point2D GetPoint(const std::vector<Point2D>& pts, float t);
    static Vector2D GetSlope(const std::vector<Point2D>& pts, float t);
    float GetMaxT() const;

private:
    std::vector<Point2D> mPoints;
};

}  // namespace CMPUT350

#endif  // BEZIER_H