#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <memory>
#include <string>
#include <vector>

#include "DrawContext.h"
#include "EngineView.h"
#include "GameContext.h"
#include "GameObject.h"
#include "MathUtil.h"
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "CollisionObject.h"
#include "NotificationManager.h"

namespace CMPUT350 {

/**
 * @brief Owns the window and all game objects, and runs the main game loop.
 *
 * Engine debug keys: 'p' toggles pause, 'n' advances a single frame while paused.
 */
class GameEngine : public EngineView {
public:
    /**
     * @brief Creates the window (limited to 30 FPS) and loads the embedded font.
     * @param width Window width in pixels.
     * @param height Window height in pixels.
     * @param name Window title.
     */
    GameEngine(unsigned int width, unsigned int height, const std::string& name);

    /**
     * @brief Closes the window if it is still open.
     */
    ~GameEngine();

    GameEngine(const GameEngine&) = delete;             // Prevent copy-construction
    GameEngine(GameEngine&&) = delete;                  // Prevent move-construction
    GameEngine& operator=(const GameEngine&) = delete;  // Prevent assignment
    GameEngine& operator=(GameEngine&&) = delete;       // Prevent move-assignment

    /**
     * @brief Queues an object to be added to the engine at the start of the next frame.
     * @param gameObject The object to add. Null pointers are ignored.
     */
    void AddGameObject(std::shared_ptr<GameObject> gameObject) override;

    /**
     * @brief Gives control to the engine. Returns once the window has been closed.
     */
    void Run();

    void InstallKeyDownNotification(int key, const std::string &notification) override;
    void InstallKeyUpNotification(int key, const std::string &notification) override;
    const_iterator cbegin() const override;
    const_iterator cend() const override;
    const_iterator begin() const override;
    const_iterator end() const override;

private:
    void RemoveDeadObjects();
    void AddPendingObjects();
    void ProcessEvents();
    void UpdateObjects();
    void ProcessCollisions();
    void LateUpdateObjects();
    void Render();
    void HandleKey(int key, bool down);
    void SetupTransform(const GraphicsObject &obj);
    Rect GetWorldBounds(CollisionObject &obj);
    std::vector<Shape> GetWorldShapes(CollisionObject &obj);
    static bool FirstContact(const Line &line, const Shape &other, Point2D &point);
    static bool ShapesOverlap(const Shape &a, const Shape &b);
    bool ObjectsCollide(CollisionObject &a, CollisionObject &b, Point2D &pointA, bool &hasPointA, Point2D &pointB, bool &hasPointB);

    std::shared_ptr<sf::RenderWindow> mWindow;
    std::shared_ptr<sf::Font> mFont;
    DrawContext mScreenContext;
    DrawContext mGUIContext;
    GameContext mContext;

    std::vector<std::shared_ptr<GameObject>> mGameObjects;     // Objects currently in the game
    std::vector<std::shared_ptr<GameObject>> mPendingObjects;  // Objects added this frame

    bool mPaused = false;
    bool mStepFrame = false;  // When paused, run exactly one more frame
    std::unordered_map<int, std::vector<std::string>> mKeyDownNotifications;
    std::unordered_map<int, std::vector<std::string>> mKeyUpNotifications;
    NotificationManager mNotificationManager;
    int mFrame = 0;


};


}  // namespace CMPUT350

#endif  // GAMEENGINE_H
