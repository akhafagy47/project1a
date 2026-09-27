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

private:
    void RemoveDeadObjects();
    void AddPendingObjects();
    void ProcessEvents();
    void UpdateObjects();
    void ProcessCollisions();
    void LateUpdateObjects();
    void Render();

    std::shared_ptr<sf::RenderWindow> mWindow;
    std::shared_ptr<sf::Font> mFont;
    DrawContext mDrawContext;  // Must be declared after mWindow and mFont (initialization order)
    GameContext mContext;

    std::vector<std::shared_ptr<GameObject>> mGameObjects;     // Objects currently in the game
    std::vector<std::shared_ptr<GameObject>> mPendingObjects;  // Objects added this frame

    bool mPaused = false;
    bool mStepFrame = false;  // When paused, run exactly one more frame
};

}  // namespace CMPUT350

#endif  // GAMEENGINE_H
