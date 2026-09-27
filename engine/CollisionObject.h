#ifndef COLLISION_OBJECT_H
#define COLLISION_OBJECT_H

#include <memory>

#include "GraphicsObject.h"
#include "MathUtil.h"

namespace CMPUT350 {

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
    virtual void CollisionEnter(const std::shared_ptr<CollisionObject> &obj) = 0;

    /**
     * @brief The object's current axis-aligned bounding box, in pixels.
     * @return Reference to the bounds. The engine copies it immediately, so it only needs to
     * stay valid until the next call to GetBounds().
     */
    virtual const Rect &GetBounds() = 0;
};

}  // namespace CMPUT350

#endif
