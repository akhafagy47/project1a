#ifndef BEZIER_H
#define BEZIER_H

#include <vector>

#include "MathUtil.h"

namespace CMPUT350 {

/**
 * @brief A chain of cubic Bezier curves. 3k+1 control points make k segments, and t runs from 0
 * to k (values outside are clamped).
 */
class Bezier {
public:
    /**
     * @brief Stores a copy of the control points.
     */
    Bezier(const std::vector<Point2D>& pts);

    /**
     * @brief Returns the point on this curve at time t.
     */
    Point2D GetPoint(float t) const;

    /**
     * @brief Returns the slope (derivative) of this curve at time t.
     */
    Vector2D GetSlope(float t) const;

    /**
     * @brief Returns the point at time t on the curve given by pts. Returns (0, 0) if there are
     * fewer than 4 points.
     */
    static Point2D GetPoint(const std::vector<Point2D>& pts, float t);

    /**
     * @brief Returns the slope at time t on the curve given by pts. Returns (0, 0) if there are
     * fewer than 4 points.
     */
    static Vector2D GetSlope(const std::vector<Point2D>& pts, float t);

    /**
     * @brief Returns the largest valid t, which is the number of cubic segments.
     */
    float GetMaxT() const;

private:
    std::vector<Point2D> mPoints;
};

}  // namespace CMPUT350

#endif  // BEZIER_H