#ifndef COLLISION_OBJECT_H
#define COLLISION_OBJECT_H
#include <memory>
#include <vector>

#include "GraphicsObject.h"
#include "MathUtil.h"

namespace CMPUT350 {

/**
 * @brief Which kind of shape a Shape holds.
 */
enum class ShapeType { kLine, kCircle, kRect };

/**
 * @brief Storage for one line, circle or axis-aligned rect. Shape::t says which one is in use.
 */
union ShapeUnion {
    ShapeUnion(Line l) : line(l) {}
    ShapeUnion(Circle c) : circle(c) {}
    ShapeUnion(Rect r) : rect(r) {}
    Line line;
    Circle circle;
    Rect rect;
};

/**
 * @brief One collision shape (line, circle or axis-aligned rect) tagged with its type.
 */
struct Shape {
    Shape(const Line& l) : shape(l), t(ShapeType::kLine) {}
    Shape(const Circle& c) : shape(c), t(ShapeType::kCircle) {}
    Shape(const Rect& r) : shape(r), t(ShapeType::kRect) {}
    ShapeUnion shape;
    ShapeType t;
};

/**
 * @brief A graphics object that can collide with other collision objects.
 *
 * Each frame the engine checks every pair once: first their bounds, then their shapes. When two
 * collide, it calls CollisionEnter() once on each of them.
 */
class CollisionObject : public GraphicsObject {
public:
    /**
     * @brief Called when this object's bounds overlap another collision object's bounds.
     * @param obj The other object in the collision (never this object).
     */
    virtual void CollisionEnter(const std::shared_ptr<CollisionObject>& obj) = 0;

    /**
     * @brief Called instead of CollisionEnter(obj) when one of this object's lines hit the other
     * object. By default it just calls CollisionEnter(obj).
     * @param obj The other object in the collision.
     * @param collisionPoint First point along the line that hit, in this object's local space.
     */
    virtual void CollisionEnter(const std::shared_ptr<CollisionObject>& obj,
                                const Point2D& /*collisionPoint*/) {
        CollisionEnter(obj);
    }

    /**
     * @brief The shapes used for collisions, in local space. If empty, the bounds are used.
     */
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
