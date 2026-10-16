#ifndef COLLISION_OBJECT_H
#define COLLISION_OBJECT_H
#include <memory>
#include <vector>

#include "GraphicsObject.h"
#include "MathUtil.h"

namespace CMPUT350 {

enum class ShapeType { kLine, kCircle, kRect };

union ShapeUnion {
    ShapeUnion(Line l) : line(l) {}
    ShapeUnion(Circle c) : circle(c) {}
    ShapeUnion(Rect r) : rect(r) {}
    Line line;
    Circle circle;
    Rect rect;
};

struct Shape {
    Shape(const Line& l) : shape(l), t(ShapeType::kLine) {}
    Shape(const Circle& c) : shape(c), t(ShapeType::kCircle) {}
    Shape(const Rect& r) : shape(r), t(ShapeType::kRect) {}
    ShapeUnion shape;
    ShapeType t;
};

/**
 * @brief A graphics object that takes part in axis-aligned bounding box (AABB) collisions.
 *
 * Each frame the engine compares the bounds of every pair of collision objects, and when two
 * overlap, calls CollisionEnter() exactly once on each of them.
 */
class CollisionObject : public GraphicsObject {
public:
    /**
     * @brief Called when this object's bounds overlap another collision object's bounds.
     * @param obj The other object in the collision (never this object).
     */
    virtual void CollisionEnter(const std::shared_ptr<CollisionObject>& obj) = 0;

    virtual void CollisionEnter(const std::shared_ptr<CollisionObject>& obj, const Point2D&) {
        CollisionEnter(obj);
    }
    virtual const std::vector<Shape>& GetShapes() = 0;

    /**
     * @brief The object's current axis-aligned bounding box, in pixels.
     * @return Reference to the bounds. The engine copies it immediately, so it only needs to
     * stay valid until the next call to GetBounds().
     */
    virtual const Rect& GetBounds() = 0;
};

}  // namespace CMPUT350

#endif
