#ifndef BULLET_H
#define BULLET_H

#include <vector>

#include "CollisionObject.h"
#include "GameContext.h"

class Bullet : public CMPUT350::CollisionObject {
public:
    /**
     * @brief Constructs a new Bullet.
     * @param location The initial coordinates of the bullet.
     * @param heading The vector describing the velocity of the bullet.
     * @param player True if shot by the player, false if shot by an enemy.
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
     * @brief Initializes the bullet..
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
     * Draws a red line extending from the bullet's previous location
     * to its current location in frame.
     *
     * @param context Pointer to the global GameContext containing the ScreenContext.
     */
    void RenderForeground(CMPUT350::GameContext* context) override;

    // Collision Object Functions

    /**
     * @brief Handles collisions with other objects.
     *
     * Bullets ignore collisions with other bullets. Bullets fired by player dont kill
     * bullets. All other collisions destroy the bullet.
     *
     * @param obj A shared pointer to the object this bullet collided with.
     */
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;

    /**
     * @brief Gets the collision bounding box of the bullet.
     * @return A constant reference to the bullet's Rect boundary.
     */
    const CMPUT350::Rect& GetBounds() override;

    CMPUT350::Point2D GetLocation() const override;
    float GetRotation() const override;
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj,
                        const CMPUT350::Point2D& collisionPoint) override;
    const std::vector<CMPUT350::Shape>& GetShapes() override;

private:
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
