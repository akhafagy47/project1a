#include "GameEngine.h"

#include <cstdio>
#include <utility>

#include "CollisionObject.h"
#include "GraphicsObject.h"

namespace CMPUT350 {
#include "FontData.h"

/**
 * @brief Returns whether two axis-aligned rectangles overlap. Touching edges count as overlap so
 * that zero-width boxes (e.g. a bullet moving straight up) still collide.
 */
static bool BoundsOverlap(const Rect& a, const Rect& b) {
    return a.topLeft.x <= b.topLeft.x + b.width && b.topLeft.x <= a.topLeft.x + a.width &&
           a.topLeft.y <= b.topLeft.y + b.height && b.topLeft.y <= a.topLeft.y + a.height;
}

GameEngine::GameEngine(unsigned int width, unsigned int height, const std::string& name)
    : mWindow(std::make_shared<sf::RenderWindow>(sf::VideoMode({width, height}), name,
                                                 sf::Style::Titlebar | sf::Style::Close)),
      mFont(std::make_shared<sf::Font>()),
      mScreenContext(mWindow, mFont),
      mGUIContext(mWindow, mFont) {
    mWindow->setFramerateLimit(30);
    mWindow->setKeyRepeatEnabled(false);

    // The font is embedded in FontData.h, so it is loaded from memory rather than from disk.
    if (!mFont->openFromMemory(_font, _font_len)) {
        fprintf(stderr, "WARNING: Font did not load.\n");
    }

    mContext.mEngineView = this;
    mContext.ScreenContext = &mScreenContext;
    mContext.GUIContext = &mGUIContext;
    mContext.mNotificationManager = &mNotificationManager;
}

GameEngine::~GameEngine() {
    // Failsafe: the window is normally closed when it receives a close event.
    if (mWindow->isOpen()) {
        mWindow->close();
    }
}

void GameEngine::AddGameObject(std::shared_ptr<GameObject> gameObject) {
    if (gameObject != nullptr) {
        mPendingObjects.push_back(std::move(gameObject));
    }
}

void GameEngine::Run() {
    while (mWindow->isOpen()) {
        // 1. Remove any objects that are now dead
        RemoveDeadObjects();

        // 2. Activate and initialize any objects added during the last frame
        AddPendingObjects();

        // 3. Process events (window close, key presses)
        ProcessEvents();
        if (!mWindow->isOpen()) {
            break;
        }

        // Steps 4-6 are skipped while paused, unless a single frame step was requested
        if (!mPaused || mStepFrame) {
            mContext.currentFrame = mFrame++;
            // 4. Update game objects
            UpdateObjects();

            // 5. Process collision events
            ProcessCollisions();

            // 6. Late updates
            LateUpdateObjects();

            mStepFrame = false;
        }

        // 7-8. Render background then foreground, 9. display the frame
        Render();
    }
}

/**
 * @brief Removes every object that is no longer alive from the main object list.
 *
 * Uses swap-and-pop: a dead object is overwritten by the last object and the vector shrinks by
 * one. This is O(n) overall and leaves no gaps, but does not preserve object order.
 */
void GameEngine::RemoveDeadObjects() {
    size_t i = 0;
    while (i < mGameObjects.size()) {
        if (!mGameObjects[i]->IsAlive()) {
            mGameObjects[i] = std::move(mGameObjects.back());
            mGameObjects.pop_back();
            // Don't advance i: the object swapped into slot i still needs to be checked
        } else {
            i++;
        }
    }
}

/**
 * @brief Moves the objects added during the last frame into the game and initializes them.
 *
 * The pending list is swapped out first, so any objects created inside Initialize() are queued
 * for the next frame instead of modifying the list being iterated.
 */
void GameEngine::AddPendingObjects() {
    std::vector<std::shared_ptr<GameObject>> newObjects;
    newObjects.swap(mPendingObjects);

    for (auto& obj : newObjects) {
        mGameObjects.push_back(obj);
        mContext.CurrObject = obj;
        obj->Initialize(&mContext);
    }
}

/**
 * @brief Handles all queued SFML events: closes the window on a close event, handles the engine
 * debug keys, and sends notifications to notification manager.
 */
void GameEngine::ProcessEvents()
{
    while (const std::optional event = mWindow->pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            mWindow->close();
        }
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->scancode == sf::Keyboard::Scancode::P)
            {
                mPaused = !mPaused;
            }
            else if (keyPressed->scancode == sf::Keyboard::Scancode::N && mPaused)
            {
                mStepFrame = true;
            }
            else
            {
                HandleKey(static_cast<int>(keyPressed->scancode), true);
            }
        }
        else if (const auto* released = event->getIf<sf::Event::KeyReleased>())
        {
            HandleKey(static_cast<int>(released->scancode), false);
        }
    }
}

/**
 * @brief Calls Update() on every game object.
 */
void GameEngine::UpdateObjects() {
    for (auto& obj : mGameObjects) {
        mContext.CurrObject = obj;
        obj->Update(&mContext);
    }
}

/**
 * @brief Checks every pair of collision objects for bounding box overlap, and calls
 * CollisionEnter() once on each object of every overlapping pair.
 *
 * Only objects that are CollisionObjects (found with RTTI) take part. Each unordered pair is
 * tested once (b > a), so objects never collide with themselves. Both objects in an overlapping
 * pair are always notified. Objects killed earlier this frame are skipped for later pairs.
 */
void GameEngine::ProcessCollisions() {
    // Collect collision objects and a copy of their bounds once, instead of casting and calling
    // GetBounds() for every pair. The bounds are copied because GetBounds() returns a reference
    // that may be overwritten by the next call (e.g. if an object returns a static Rect).
    std::vector<std::pair<std::shared_ptr<CollisionObject>, Rect>> colliders;
    for (auto& obj : mGameObjects) {
        std::shared_ptr<CollisionObject> collider = std::dynamic_pointer_cast<CollisionObject>(obj);
        if (collider == nullptr) {
            continue;  // Not a collision object, skip
        }
        Rect bounds = collider->GetBounds();
        colliders.emplace_back(collider, bounds);
    }

    for (size_t a = 0; a < colliders.size(); a++) {
        for (size_t b = a + 1; b < colliders.size(); b++) {
            auto& [objA, boundsA] = colliders[a];
            auto& [objB, boundsB] = colliders[b];

            if (!objA->IsAlive()) {
                break;  // objA can't collide with anything else this frame
            }
            if (!objB->IsAlive() || !BoundsOverlap(boundsA, boundsB)) {
                continue;
            }

            objA->CollisionEnter(objB);
            objB->CollisionEnter(objA);
        }
    }
}

/**
 * @brief Calls LateUpdate() on every game object.
 */
void GameEngine::LateUpdateObjects() {
    for (auto& obj : mGameObjects) {
        mContext.CurrObject = obj;
        obj->LateUpdate(&mContext);
    }
}

/**
 * @brief Draws the frame: clears the window, renders every graphics object's background, then
 * every foreground, then every object's UI, and finally displays the result.
 *
 * Separate loops guarantee all backgrounds are behind all foregrounds regardless of the order
 * objects are stored in.
 */
void GameEngine::Render() {
    mWindow->clear(sf::Color::Black);

    for (auto& obj : mGameObjects) {
        if (auto graphics = std::dynamic_pointer_cast<GraphicsObject>(obj)) {
            graphics->RenderBackground(&mContext);
        }
    }

    for (auto& obj : mGameObjects) {
        if (auto graphics = std::dynamic_pointer_cast<GraphicsObject>(obj)) {
            graphics->RenderForeground(&mContext);
        }
    }

    for (auto& obj : mGameObjects) {
        obj->RenderUI(&mContext);
    }

    mWindow->display();
}

void GameEngine::InstallKeyDownNotification(int key, const std::string& notification)
{
    mKeyDownNotifications[key].push_back(notification);
}
void GameEngine::InstallKeyUpNotification(int key, const std::string& notification)
{
    mKeyUpNotifications[key].push_back(notification);
}

void GameEngine::HandleKey(int key, bool down)
{
    auto* key_map = &mKeyUpNotifications;
    if (down)
    {
        key_map = &mKeyDownNotifications;
    }
    auto found_entry = key_map->find(key);

    if (found_entry == key_map->end())
    {
        return;
    }
    for (const std::string& notification : found_entry->second)
    {
        mNotificationManager.Notify(notification);
    }
}

EngineView::const_iterator GameEngine::cbegin() const { return mGameObjects.cbegin(); }
EngineView::const_iterator GameEngine::cend() const { return mGameObjects.cend(); }
EngineView::const_iterator GameEngine::begin() const { return mGameObjects.begin(); }
EngineView::const_iterator GameEngine::end() const { return mGameObjects.end(); }

}  // namespace CMPUT350
