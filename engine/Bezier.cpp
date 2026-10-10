#include "Bezier.h"

namespace CMPUT350 {
Bezier::Bezier(const std::vector<Point2D> &pts) : mPoints(pts) {}
Point2D Bezier::GetPoint(float t) const { return Point2D(0, 0); }
Point2D Bezier::GetSlope(float t) const { return Point2D(0, 0); }
Point2D Bezier::GetPoint(const std::vector<Point2D> &pts, float t) { return Point2D(0, 0); }
Point2D Bezier::GetSlope(const std::vector<Point2D> &pts, float t) { return Point2D(0, 0); }
float Bezier::GetMaxT() const { return mPoints.empty() ? 0.0f : (mPoints.size() - 1) / 3.0f; }
}  // namespace CMPUT350
