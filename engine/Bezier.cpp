#include "Bezier.h"

namespace CMPUT350 {

Bezier::Bezier(const std::vector<Point2D> &pts) : mPoints(pts) {}

Point2D Bezier::GetPoint(float t) const {
   return GetPoint(mPoints, t);
}
Point2D Bezier::GetSlope(float t) const {
    return GetSlope(mPoints, t);
}

Point2D Bezier::GetPoint(const std::vector<Point2D> &pts, float t){
    if (pts.size() < 4){
        return Point2D(0, 0);
    }

    return (1-t)*(1-t)*(1-t)*pts[0] + 3*(1-t)*(1-t)*t*pts[1] + 3*(1-t)*t*t*pts[2] + t*t*t*pts[3];
}

Point2D Bezier::GetSlope(const std::vector<Point2D> &pts, float t){
    if (pts.size() < 4)
    {
        return Point2D(0, 0);
    }

    return 3*(1-t)*(1-t)*(pts[1] - pts[0]) + 6*(1-t)*t*(pts[2] - pts[1]) + 3*t*t*(pts[3] - pts[2]);
}

float Bezier::GetMaxT() const
{
    if (mPoints.empty()) {
        return 0.0f;
    } else {
        return (mPoints.size() - 1) / 3.0f;
    }
}
}  // namespace CMPUT350
