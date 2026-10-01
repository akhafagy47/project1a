#ifndef ENEMY_H
#define ENEMY_H

#include "CollisionObject.h"
#include "GameContext.h"


class Enemy : public CMPUT350::CollisionObject
{
public:

    /**
     * @brief Constructs a new Enemy.
     * @param loc The inital center coordinates of the enemy.
     */
    Enemy(CMPUT350::Point2D loc);

    // GameObject Functions


    /**
     * @brief Initializes the enemy object.
     * @param context Pointer to global GameContext.
     */
    void Initialize(CMPUT350::GameContext* context) override;

    /**
     * @brief Updates enemy state for the current frame.
     * @param context Pointer to global GameContext.
     */
    void Update(CMPUT350::GameContext* context) override;

    /**
     * @brief Late update phase for the enemy.
     * @param context Pointer to the global GameContext.
     */
    void LateUpdate(CMPUT350::GameContext* context) override;

    /**
     * @brief Handles keyboard input.
     * @param context Pointer to global GameContext.
     * @param key The character pressed.
     * @return false because enemies do not respond to direct keyboard input.
     */
    bool HandleKeyEvent(CMPUT350::GameContext* context, char key) override;

    /**
     * @brief Checks if the enemy is currently alive.
     * @return true if the enemy is alive but false if destroyed.
     */
    bool IsAlive() const override;

    /**
     * @brief Flags the enemy to be removed from the game.
     */
    void Kill() override;

    // Graphics Object Functions

    /**
     * @brief Renders the background elements of the enemy.
     * @param context Pointer to global GameContext.
     */
    void RenderBackground(CMPUT350::GameContext* context) override;

    /**
     * @brief Renders the enemy to the screen.
     * 
     * Draws a blue rectangle representing the bounding box of enemy.
     * 
     * @param context Pointer to global GameContext that contains the ScreenContext.
     */
    void RenderForeground(CMPUT350::GameContext* context) override;


    // Collision Object Functions
    
    /**
     * @brief Handles collision events with other objects.
     * 
     * Ignores collisions with bullets fired by other enemies. 
     * Any collision with a bullet fired by player kills enemy.
     * 
     * @param obj A shared pointer to the object the enemy collides with.
     */
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;
    
    /**
     * @brief Gets the collision bounding box of the enemy.
     * @return A constant reference to the enemy's Rect boundary.
     */
    const CMPUT350::Rect& GetBounds() override;
private:
    bool is_enemy_alive;
    CMPUT350::Rect enemy_boundary;
    CMPUT350::Point2D enemy_loc;
};


#endif

