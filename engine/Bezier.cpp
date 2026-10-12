#include "Bezier.h"
#include <algorithm>
#include <cmath>

namespace CMPUT350 {

Bezier::Bezier(const std::vector<Point2D> &pts) : mPoints(pts) {}

Point2D Bezier::GetPoint(float t) const {
   return GetPoint(mPoints, t);
}
Point2D Bezier::GetSlope(float t) const {
    return GetSlope(mPoints, t);
}

Point2D Bezier::GetPoint(const std::vector<Point2D> &pts, float t){
    if (pts.size() < 4)
    {
        return Point2D(0, 0);
    }

    size_t segments = (pts.size() - 1)/3;
    t = std::max(0.0f, std::min(t, static_cast<float>(segments)));

    size_t i = std::min(static_cast<size_t>(std::floor(t)), segments - 1);
    float u = t - static_cast<float>(i);

    size_t first = 3*i;

    return (1-u)*(1-u)*(1-u)*pts[first] + 3*(1-u)*(1-u)*u*pts[first + 1] + 3*(1-u)*u*u*pts[first + 2] + u*u*u*pts[first + 3];
}

Point2D Bezier::GetSlope(const std::vector<Point2D> &pts, float t){
    if (pts.size() < 4)
    {
        return Point2D(0, 0);
    }

    size_t segments = (pts.size() - 1)/3;
    t = std::max(0.0f, std::min(t, static_cast<float>(segments)));

    size_t i = std::min(static_cast<size_t>(std::floor(t)), segments - 1);
    float u = t - static_cast<float>(i);
    size_t first = 3*i;

    return 3*(1-u)*(1-u)*(pts[first + 1] - pts[first]) + 6*(1-u)*u*(pts[first + 2] - pts[first + 1]) + 3*u*u*(pts[first + 3] - pts[first + 2]);
}

float Bezier::GetMaxT() const
{
    if (mPoints.empty())
    {
        return 0.0f;
    }
    else
    {
        return static_cast<float>((mPoints.size()- 1)/3);
    }
}
}  // namespace CMPUT350
