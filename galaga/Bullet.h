#ifndef BULLET_H
#define BULLET_H

#include <vector>

#include "CollisionObject.h"
#include "GameContext.h"

/**
 * @brief A bullet fired by the player or an enemy. It collides as a directed line from its
 * previous position to its current one.
 */
class Bullet : public CMPUT350::CollisionObject {
public:
    /**
     * @brief Constructs a new Bullet.
     * @param location The initial coordinates of the bullet.
     * @param heading The vector describing the velocity of the bullet.
     * @param player True if shot by the player, false if shot by an enemy.
     * @param parent The object that fired the bullet. The bullet dies when the parent does.
     */
    Bullet(CMPUT350::Point2D location, CMPUT350::Vector2D heading, bool player,
           std::weak_ptr<CMPUT350::GameObject> parent);

    /**
     * @brief Checks the origin of the bullet.
     * @return true if the bullet was fired by the player, false otherwise.
     */
    bool IsPlayerBullet();

    // GameObject Functions

    /**
     * @brief Initializes the bullet.
     * @param context Pointer to global GameContext.
     */
    void Initialize(CMPUT350::GameContext* context) override;

    /**
     * @brief Updates the bullet's position and bounding box for the current frame.
     *
     * Moves the bullet according to its heading vector. If the bullet
     * exits the screen, it gets killed.
     *
     * @param context Pointer to the global GameContext to get screen boundary.
     */
    void Update(CMPUT350::GameContext* context) override;

    /**
     * @brief Late update phase for the bullet.
     * @param context Pointer to global GameContext.
     */
    void LateUpdate(CMPUT350::GameContext* context) override;

    /**
     * @brief Checks if bullet is alive in the game.
     * @return true if the bullet is alive but false if it has been destroyed.
     */
    bool IsAlive() const override;

    /**
     * @brief Sets flag so that it gets destroyed in next frame.
     */
    void Kill() override;

    // Graphics Object Functions

    /**
     * @brief Renders the background of the bullet.
     * @param context Pointer to global GameContext.
     */
    void RenderBackground(CMPUT350::GameContext* context) override;

    /**
     * @brief Renders bullet on the screen.
     *
     * Draws a line from the bullet's previous location to its current one (red for the player,
     * yellow for enemies). After a hit, draws a circle at the hit point instead.
     *
     * @param context Pointer to the global GameContext containing the ScreenContext.
     */
    void RenderForeground(CMPUT350::GameContext* context) override;

    // Collision Object Functions

    /**
     * @brief Handles collisions with other objects.
     *
     * Same as the other CollisionEnter, with the hit at the bullet's current location.
     *
     * @param obj A shared pointer to the object this bullet collided with.
     */
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;

    /**
     * @brief Handles a collision at a given point.
     *
     * Ignores other bullets, the player for player bullets, and enemies for enemy bullets. Any
     * other hit stops the bullet at the collision point (the one with the largest y if there are
     * several) and kills it.
     *
     * @param obj A shared pointer to the object this bullet collided with.
     * @param collisionPoint Where the hit happened, in the bullet's local space.
     */
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj,
                        const CMPUT350::Point2D& collisionPoint) override;

    /**
     * @brief Gets the collision bounding box of the bullet.
     * @return A constant reference to the bullet's Rect boundary.
     */
    const CMPUT350::Rect& GetBounds() override;

    /**
     * @brief Gets the bullet's collision shape: one line from its previous location to its
     * current one.
     */
    const std::vector<CMPUT350::Shape>& GetShapes() override;

    /**
     * @brief Gets the bullet's current location.
     */
    CMPUT350::Point2D GetLocation() const override;

    /**
     * @brief Gets the bullet's rotation, which is always 0.
     */
    float GetRotation() const override;

private:
    /**
     * @brief Rebuilds the collision line and bounds from the previous and current location.
     */
    void UpdateCollisionData();
    bool mIsAlive;
    bool mShotByPlayer;
    bool mHasHit;
    CMPUT350::Point2D mLocation;
    CMPUT350::Point2D mPreviousLocation;
    CMPUT350::Vector2D mHeading;
    CMPUT350::Rect mBounds;

    CMPUT350::Point2D mHitPoint;
    std::weak_ptr<CMPUT350::GameObject> mParent;
    std::vector<CMPUT350::Shape> mShapes;
};
#endif  // BULLET_H
