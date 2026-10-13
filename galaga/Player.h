#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <vector>
#include <memory>

class Bullet;

#include "CollisionObject.h"

class Player : public CMPUT350::CollisionObject
{
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
     * @brief Updates the bullet's position and bounding box for the current frame.
     * 
     * Makes sure player doesnt move outside the and
     * updates collision rectangle based on current location.
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

    void ReceiveNotification(const std::string &key) override;
    CMPUT350::Point2D GetLocation() const override;
    float GetRotation() const override;
    const std::vector<CMPUT350::Shape> &GetShapes() override;

    //used as reference: https://en.cppreference.com/w/cpp/language/constexpr
    static constexpr const char* shift_left_key = "ShiftLeft";
    static constexpr const char* shift_right_key = "ShiftRight";
    static constexpr const char* stop_left_key = "StopLeft";
    static constexpr const char* stop_right_key = "StopRight";
    static constexpr const char* fire_key = "FireBullet";
private:
    void FireBullet(CMPUT350::GameContext* context);
    bool is_player_alive;
    bool shifting_left;
    bool shifting_right;
    bool requesting_fire;
    CMPUT350::Rect player_boundary;
    CMPUT350::Point2D player_loc;
    std::weak_ptr<Bullet> player_bullet_1;
    std::weak_ptr<Bullet> player_bullet_2;
    std::vector<CMPUT350::Shape> player_shapes;




};

#endif
