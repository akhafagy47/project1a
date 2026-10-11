#ifndef ENEMY_H
#define ENEMY_H

#include <string>
#include <vector>

#include "CollisionObject.h"
#include "DrawContext.h"
#include "GameContext.h"

/**
 * @brief Base class for all enemies: follows Bezier paths at a constant speed, hovers in
 * formation, attacks when asked, takes hits and reports its death.
 *
 * Lifecycle: Entering -> InFormation <-> Attacking -> Returning -> InFormation.
 * An enemy enters along the path given to its constructor, then joins its formation slot. When
 * Galaga calls Attack() it flies its type's attack path, then flies back to wherever the
 * formation is at that moment.
 *
 * Enemies draw in local space with the nose pointing down (+y); the engine applies the location
 * and rotation returned by GetLocation() and GetRotation().
 */
class Enemy : public CMPUT350::CollisionObject
{
public:
    enum class State
    {
        Entering,
        InFormation,
        Attacking,
        Returning
    };

    /**
     * @brief Constructs an enemy.
     * @param home The enemy's slot in the formation (screen coordinates).
     * @param entryPath Bezier control points (3k+1 points) the enemy follows onto the screen. The
     * enemy starts at the first point. An empty or invalid path places it straight in formation.
     */
    Enemy(CMPUT350::Point2D home, std::vector<CMPUT350::Point2D> entryPath);

    /**
     * @brief Starts following the entry path, with its end adjusted to where the hovering
     * formation slot will be when the enemy arrives.
     * @param context Pointer to global GameContext.
     */
    void Initialize(CMPUT350::GameContext* context) override;

    /**
     * @brief Moves the enemy along its current path or hovers it in formation, and fires
     * bullets at scheduled points of an attack.
     * @param context Pointer to global GameContext.
     */
    void Update(CMPUT350::GameContext* context) override;

    /**
     * @brief Applies the hits received this frame. When health reaches 0, sends this type's
     * "killed" notification and kills the enemy.
     * @param context Pointer to global GameContext.
     */
    void LateUpdate(CMPUT350::GameContext* context) override;

    /**
     * @brief Records a hit from the player or a player bullet; everything else is ignored.
     *
     * There is no GameContext here, so the damage is only counted and applied in LateUpdate().
     * @param obj The object the enemy collided with.
     */
    void CollisionEnter(const std::shared_ptr<CMPUT350::CollisionObject>& obj) override;

    /**
     * @brief The enemy's world-space bounding box, accounting for its rotation.
     * @return Reference to the bounds, valid until the next call.
     */
    const CMPUT350::Rect& GetBounds() override;

    /**
     * @brief The collision shapes in local space: one rectangle centered on (0, 0).
     * @return Reference to the shape list.
     */
    const std::vector<CMPUT350::Shape>& GetShapes() override;

    /**
     * @brief The enemy's center in screen coordinates, which is also its rotation point.
     * @return The current location.
     */
    CMPUT350::Point2D GetLocation() const override;

    /**
     * @brief The enemy's rotation in radians. 0 means the nose points straight down.
     * @return The current rotation.
     */
    float GetRotation() const override;

    /**
     * @brief Asks the enemy to leave formation and attack.
     * @param context Pointer to global GameContext.
     * @param target The point to attack, normally the player's location.
     * @return true if the attack started, false if the enemy is not in formation.
     */
    virtual bool Attack(CMPUT350::GameContext* context, const CMPUT350::Point2D& target) = 0;

    /**
     * @brief Whether the enemy is currently hovering in its formation slot.
     * @return true if in formation.
     */
    bool IsInFormation() const;

    /**
     * @brief The enemy's main color, which may change as it loses health.
     * @return The color to draw the enemy's body with.
     */
    virtual CMPUT350::RGBColor Color() const = 0;

protected:
    /**
     * @brief The type name used in notifications, e.g. "Enemy1".
     * @return The name; the killed messages are name + "KilledInFormation" and
     * name + "KilledOutOfFormation".
     */
    virtual std::string TypeName() const = 0;

    /**
     * @brief Called when the attack path ends, before the enemy flies back to formation.
     * Subclasses can move the enemy here (e.g. wrap it to the top of the screen).
     */
    virtual void OnAttackPathFinished();

    /**
     * @brief Starts an attack along the given path. Called by subclasses' Attack().
     * @param path Bezier control points (3k+1 points), starting at the enemy's location.
     * @param fireAt Fractions of the path's length (0 to 1, increasing) at which to fire.
     * @param target The point bullets are aimed at.
     */
    void StartAttack(std::vector<CMPUT350::Point2D> path, std::vector<float> fireAt,
                     CMPUT350::Point2D target);

    /**
     * @brief Sets the enemy's starting (and maximum) health. Call from Initialize().
     * @param health Number of player-bullet hits needed to destroy the enemy.
     */
    void SetHealth(int health);

    /**
     * @brief The enemy's remaining health.
     * @return Hits left before the enemy is destroyed.
     */
    int Health() const;

    /**
     * @brief The enemy's starting health.
     * @return The health set with SetHealth().
     */
    int MaxHealth() const;

    /**
     * @brief The window size, falling back to the Galaga screen size if no context is available.
     * @param context Pointer to global GameContext.
     * @return Width and height in pixels.
     */
    static CMPUT350::Point2D ScreenSize(CMPUT350::GameContext* context);

    CMPUT350::Point2D mLocation;
    CMPUT350::Point2D mHeading;  // Unit direction of travel, kept after a path ends
    float mSpeed = 6.0f;         // Pixels per frame along a path

private:
    void StartPath(std::vector<CMPUT350::Point2D> path);
    bool AdvanceAlongPath();
    void StartJoinFormation(CMPUT350::GameContext* context);
    void FireIfScheduled(CMPUT350::GameContext* context);
    CMPUT350::Point2D HoverPosition(int frame) const;

    State mState = State::Entering;
    CMPUT350::Point2D mHome;
    std::vector<CMPUT350::Point2D> mEntryPath;
    float mRotation = 0.0f;

    // Current path and its arc-length table (cumulative length at each sampled t)
    std::vector<CMPUT350::Point2D> mPath;
    std::vector<float> mSampleT;
    std::vector<float> mSampleLength;
    float mDistance = 0.0f;  // Distance travelled along the current path

    // Attack bookkeeping
    CMPUT350::Point2D mTarget;
    std::vector<float> mFireAt;
    size_t mNextFire = 0;

    int mHealth = 1;
    int mMaxHealth = 1;
    int mPendingHits = 0;

    CMPUT350::Rect mBounds;
    std::vector<CMPUT350::Shape> mShapes;
};

/**
 * @brief "Bee": 1 health, yellow. Dives straight down at the target's x position, leaves the
 * bottom of the screen and re-enters from the top.
 */
class Enemy1 : public Enemy
{
public:
    Enemy1(CMPUT350::Point2D home, const std::vector<CMPUT350::Point2D>& path) : Enemy(home, path) {}

    /**
     * @brief Sets health to 1, then starts the entry path.
     * @param context Pointer to global GameContext.
     */
    void Initialize(CMPUT350::GameContext* context) override;

    /**
     * @brief Dives at the target's x position. See Enemy::Attack().
     */
    bool Attack(CMPUT350::GameContext* context, const CMPUT350::Point2D& target) override;

    /**
     * @brief Draws the bee in local space.
     * @param context Pointer to global GameContext.
     */
    void RenderForeground(CMPUT350::GameContext* context) override;

    CMPUT350::RGBColor Color() const override;

protected:
    std::string TypeName() const override;

    /**
     * @brief Wraps the enemy from below the screen to just above it.
     */
    void OnAttackPathFinished() override;
};

/**
 * @brief "Butterfly": 1 health, red. Zigzags down toward the target in S-curves.
 */
class Enemy2 : public Enemy
{
public:
    Enemy2(CMPUT350::Point2D home, const std::vector<CMPUT350::Point2D>& path) : Enemy(home, path) {}

    /**
     * @brief Sets health to 1, then starts the entry path.
     * @param context Pointer to global GameContext.
     */
    void Initialize(CMPUT350::GameContext* context) override;

    /**
     * @brief Zigzags down toward the target. See Enemy::Attack().
     */
    bool Attack(CMPUT350::GameContext* context, const CMPUT350::Point2D& target) override;

    /**
     * @brief Draws the butterfly in local space.
     * @param context Pointer to global GameContext.
     */
    void RenderForeground(CMPUT350::GameContext* context) override;

    CMPUT350::RGBColor Color() const override;

protected:
    std::string TypeName() const override;
};

/**
 * @brief "Boss": 2 health, green, turning magenta once damaged. Flies a wide loop around the
 * target, then climbs back to formation.
 */
class Enemy3 : public Enemy
{
public:
    Enemy3(CMPUT350::Point2D home, const std::vector<CMPUT350::Point2D>& path) : Enemy(home, path) {}

    /**
     * @brief Sets health to 2, then starts the entry path.
     * @param context Pointer to global GameContext.
     */
    void Initialize(CMPUT350::GameContext* context) override;

    /**
     * @brief Loops around the target. See Enemy::Attack().
     */
    bool Attack(CMPUT350::GameContext* context, const CMPUT350::Point2D& target) override;

    /**
     * @brief Draws the boss in local space.
     * @param context Pointer to global GameContext.
     */
    void RenderForeground(CMPUT350::GameContext* context) override;

    CMPUT350::RGBColor Color() const override;

protected:
    std::string TypeName() const override;
};

#endif  // ENEMY_H
