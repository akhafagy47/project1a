#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <string>
#include <vector>

class Bullet;

#include "CollisionObject.h"

/**
 * @brief The player's ship. Moves left and right and fires through key notifications.
 */
class Player : public CMPUT350::CollisionObject {
public:
    /**
     * @brief Constructs a new Player.
     * @param loc The initial center coordinates of the player.
     */
    Player(CMPUT350::Point2D loc);

    // GameObject Functions

    /**
     * @brief Initializes the player object.
     * @param context Pointer to global GameContext.
     */
    void Initialize(CMPUT350::GameContext* context) override;

    /**
     * @brief Moves the player for the current frame and fires if requested.
     *
     * Keeps the player inside the screen and fires a bullet if the fire key was pressed.
     *
     * @param context Pointer to the global GameContext to get screen boundary.
     */
    void Update(CMPUT350::GameContext* context) override;

    /**
     * @brief Late update phase for the player.
     * @param context Pointer to global GameContext.
     */
    void LateUpdate(CMPUT350::GameContext* context) override;

    /**
     * @brief Checks if the player is currently alive.
     * @return true if the player is alive but false if not alive.
     */
    bool IsAlive() const override;

    /**
     * @brief Flags the player to be destroyed in the game.
     */
    void Kill() override;

    // Graphics Object Functions

    /**
     * @brief Renders the background elements of the player.
     * @param context Pointer to global GameContext.
     */
    void RenderBackground(CMPUT350::GameContext* context) override;

    /**
     * @brief Renders the player ship on the screen.
     *
     * Draws main body, wings, cannons, and gun using rectangles.
     *
     * @param context Pointer to global GameContext containing the ScreenContext.
     */
    void RenderForeground(CMPUT350::GameContext* context) override;

    // Collision Object Functions

    /**
     * @brief Handles collision with other objects.
     *
     * Ignores collisions with the player's own bullets. Collision with enemy
     * or bullet fired by them destroys player.
     *
     * @param obj A shared pointer to the object the player collided with.
     */
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;

    /**
     * @brief Gets the collision bounding box of the player.
     * @return A constant reference to the player's Rect boundary.
     */
    const CMPUT350::Rect& GetBounds() override;

    /**
     * @brief Starts or stops moving, or requests a shot, depending on the notification.
     * @param key One of the notification names below.
     */
    void ReceiveNotification(const std::string& key) override;

    /**
     * @brief Gets the center of the player.
     */
    CMPUT350::Point2D GetLocation() const override;

    /**
     * @brief Gets the player's rotation, which is always 0.
     */
    float GetRotation() const override;

    /**
     * @brief Gets the player's collision shapes (body and wings), in local space.
     */
    const std::vector<CMPUT350::Shape>& GetShapes() override;

    // Notification names for the player's keys. Galaga installs them on the engine.
    // used as reference: https://en.cppreference.com/w/cpp/language/constexpr
    static constexpr const char* kShiftLeftKey = "ShiftLeft";
    static constexpr const char* kShiftRightKey = "ShiftRight";
    static constexpr const char* kStopLeftKey = "StopLeft";
    static constexpr const char* kStopRightKey = "StopRight";
    static constexpr const char* kFireKey = "FireBullet";

private:
    /**
     * @brief Fires a bullet from the gun, unless two player bullets are already alive.
     */
    void FireBullet(CMPUT350::GameContext* context);
    bool mIsAlive;
    bool mShiftingLeft;
    bool mShiftingRight;
    bool mRequestingFire;
    CMPUT350::Rect mBounds;
    CMPUT350::Point2D mLocation;
    std::weak_ptr<Bullet> mBullet1;
    std::weak_ptr<Bullet> mBullet2;
    std::vector<CMPUT350::Shape> mShapes;
};

#endif
