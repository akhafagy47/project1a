#include "GameEngine.h"
#include <algorithm>
#include <limits>
#include <cmath>

#include <cstdio>
#include <utility>
#include <vector>
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
void GameEngine::ProcessEvents(){
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
 * @brief Goes through all the game objects to check for collisions.
 *
 * It loops through every pair of objects exactly once. If both objects are alive 
 * and touching, it calls CollisionEnter() on both of them so they know they got hit. 
 * If a line shape hits something, it also passes the exact point of impact.
 */
void GameEngine::ProcessCollisions() {
    // Collect collision objects and a copy of their bounds once, instead of casting and calling
    // GetBounds() for every pair. The bounds are copied because GetBounds() returns a reference
    // that may be overwritten by the next call (e.g. if an object returns a static Rect).
    std::vector<std::shared_ptr<CollisionObject>> colliders;
    for (auto& obj : mGameObjects) {
        std::shared_ptr<CollisionObject> collider = std::dynamic_pointer_cast<CollisionObject>(obj);
        if (collider != nullptr) {
            colliders.push_back(collider);
        }
    }

    for (size_t a = 0; a < colliders.size(); a++) {
        for (size_t b = a + 1; b < colliders.size(); b++) {
            auto& objA = colliders[a];
            auto& objB = colliders[b];

            if (!objA->IsAlive()) {
                break; 
            }
            if (!objB->IsAlive()) {
                continue;
            }

            Point2D pointA;
            Point2D pointB;
            bool containsPointA = false;
            bool containsPointB = false;

            if (!ObjectsCollide(*objA, *objB, pointA, containsPointA, pointB, containsPointB)) {
                continue;
            }

            mContext.CurrObject = objA;
            if (containsPointA) {
                objA->CollisionEnter(objB, pointA);
            } else {
                objA->CollisionEnter(objB);
            }

            mContext.CurrObject = objB;
            if (containsPointB) {
                objB->CollisionEnter(objA, pointB);
            } else {
                objB->CollisionEnter(objA);
            }
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
            mContext.CurrObject = obj;
            SetupTransform(*graphics);
            graphics->RenderBackground(&mContext);
        }
    }

    for (auto& obj : mGameObjects) {
        if (auto graphics = std::dynamic_pointer_cast<GraphicsObject>(obj)) {
            mContext.CurrObject = obj;
            SetupTransform(*graphics);
            graphics->RenderForeground(&mContext);
        }
    }

    for (auto& obj : mGameObjects) {
        mContext.CurrObject = obj;
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


void GameEngine::SetupTransform(const GraphicsObject &obj) {
    mContext.ScreenContext->SetContextRotation(obj.GetRotation());
    mContext.ScreenContext->SetContextOffset(obj.GetLocation());
}

Rect GameEngine::GetWorldBounds(CollisionObject &obj) {
    SetupTransform(obj);
    return mContext.ScreenContext->Transform(obj.GetBounds());
}

std::vector<Shape> GameEngine::GetWorldShapes(CollisionObject &obj) {
    SetupTransform(obj);
    std::vector<Shape> worldTransformedShape;
    const std::vector<Shape>& localSpaceShapes = obj.GetShapes();
    
    //objects without shape can still be hit as they are treated as rectangles covering bounds
    if (localSpaceShapes.empty()) {
        worldTransformedShape.push_back(Shape(mContext.ScreenContext->Transform(obj.GetBounds())));
        return worldTransformedShape;
    }
    for (const Shape& currentShape : localSpaceShapes) {
        if (currentShape.t == ShapeType::kCircle) {
            Point2D interimCenter = mContext.ScreenContext->Transform(currentShape.shape.circle.center);
            worldTransformedShape.push_back(Shape(Circle(interimCenter, currentShape.shape.circle.radius)));
        } 
        else if (currentShape.t == ShapeType::kRect) {
            Rect interimRect = mContext.ScreenContext->Transform(currentShape.shape.rect);
            worldTransformedShape.push_back(Shape(interimRect));
        }
        else if (currentShape.t == ShapeType::kLine) {
            Point2D interimP1 = mContext.ScreenContext->Transform(currentShape.shape.line.p1);
            Point2D interimP2 = mContext.ScreenContext->Transform(currentShape.shape.line.p2);
            worldTransformedShape.push_back(Shape(Line(interimP1, interimP2)));
        }
    }
    return worldTransformedShape;
}

bool GameEngine::FirstContact(const Line& line, const Shape& other, Point2D& point) {
    Point2D lineDirection = line.p2 - line.p1;

    if (other.t == ShapeType::kLine) {
        return line.Crosses(other.shape.line, point);
    }

    if (other.t == ShapeType::kCircle) {
        const Circle& targetCircle = other.shape.circle;
        Point2D offsetFromCenter = line.p1 - targetCircle.center;

        float quadC = offsetFromCenter.Dot(offsetFromCenter) - targetCircle.radius*targetCircle.radius;
        if (quadC <= 0.0f) {
            point = line.p1;
            return true;
        }

        float quadA = lineDirection.Dot(lineDirection);
        if (quadA == 0.0f) {
            return false; 
        }
        
        float quadB = 2.0f*offsetFromCenter.Dot(lineDirection);
        float discriminant = quadB*quadB - 4.0f*quadA*quadC;
        
        if (discriminant < 0.0f) {
            return false; 
        }

        float entryTime = (-quadB - std::sqrt(discriminant))/(2.0f*quadA); 
        if (entryTime < 0.0f || entryTime > 1.0f) {
            return false; 
        }
        
        point = line.p1 + (entryTime*lineDirection);
        return true;
    }

    if (other.t == ShapeType::kRect) {
        const Rect& targetRect = other.shape.rect;
        
        float minX = targetRect.topLeft.x;
        float minY = targetRect.topLeft.y;
        float maxX = targetRect.topLeft.x + targetRect.width;
        float maxY = targetRect.topLeft.y + targetRect.height;
        
        float startX = line.p1.x;
        float startY = line.p1.y;
        
        float deltaX = lineDirection.x;
        float deltaY = lineDirection.y;
        float timeToEnter = 0.0f;
        float timeToExit = 1.0f;

        if (std::fabs(deltaX) < 1e-9f) {
            if (startX < minX || startX > maxX) {
                return false;
            }
        } else {
            float timeOneX = (minX - startX)/deltaX;
            float timeTwoX = (maxX - startX)/deltaX;
            
            if (timeOneX > timeTwoX) {
                float temp = timeOneX;
                timeOneX = timeTwoX;
                timeTwoX = temp;
            }
            
            if (timeOneX > timeToEnter) {
                timeToEnter = timeOneX;
            }
            if (timeTwoX < timeToExit) {
                timeToExit = timeTwoX;
            }
            if (timeToEnter > timeToExit) {
                return false;
            }
        }

        if (std::fabs(deltaY) < 1e-9f) {
            if (startY < minY || startY > maxY) {
                return false;
            }
        } else {
            float timeOneY = (minY - startY)/deltaY;
            float timeTwoY = (maxY - startY)/deltaY;
            
            if (timeOneY > timeTwoY) {
                float temp = timeOneY;
                timeOneY = timeTwoY;
                timeTwoY = temp;
            }
            
            if (timeOneY > timeToEnter) {
                timeToEnter = timeOneY;
            }
            if (timeTwoY < timeToExit) {
                timeToExit = timeTwoY;
            }
            if (timeToEnter > timeToExit) {
                return false;
            }
        }


        point = line.p1 + (timeToEnter*lineDirection);
        return true;
    }
    return false;
}

bool GameEngine::ShapesOverlap(const Shape& shapeOne, const Shape& shapeTwo) {
    Point2D dummyContactPoint;

    
    if (shapeOne.t == ShapeType::kLine) {
        return FirstContact(shapeOne.shape.line, shapeTwo, dummyContactPoint);
    }
    if (shapeTwo.t == ShapeType::kLine) {
        return FirstContact(shapeTwo.shape.line, shapeOne, dummyContactPoint);
    }

    if (shapeOne.t == ShapeType::kCircle && shapeTwo.t == ShapeType::kCircle) {
        float combinedRadii = shapeOne.shape.circle.radius + shapeTwo.shape.circle.radius;
        return shapeOne.shape.circle.center.Distance(shapeTwo.shape.circle.center) <= combinedRadii;
    }

    if (shapeOne.t == ShapeType::kRect && shapeTwo.t == ShapeType::kRect) {
        return BoundsOverlap(shapeOne.shape.rect, shapeTwo.shape.rect);
    }

    Circle collisionCircle;
    Rect collisionRect;

    if (shapeOne.t == ShapeType::kCircle) {
        collisionCircle = shapeOne.shape.circle;
        collisionRect = shapeTwo.shape.rect;
    } else {
        collisionCircle = shapeTwo.shape.circle;
        collisionRect = shapeOne.shape.rect;
    }

    float rectLeftX = collisionRect.topLeft.x;
    float rectRightX = collisionRect.topLeft.x + collisionRect.width;
    float closestX = std::max(rectLeftX, std::min(collisionCircle.center.x, rectRightX));

    float rectTopY = collisionRect.topLeft.y;
    float rectBottomY = collisionRect.topLeft.y + collisionRect.height;
    float closestY = std::max(rectTopY, std::min(collisionCircle.center.y, rectBottomY));

    Point2D closestPointOnRect(closestX, closestY);
    if (collisionCircle.center.Distance(closestPointOnRect) <= collisionCircle.radius){
        return true;
    }
    else{
        return false;
    }
}

bool GameEngine::ObjectsCollide(CollisionObject& a, CollisionObject& b, Point2D& pointA, bool& hasPointA, Point2D& pointB, bool& hasPointB) {
    hasPointA = false;
    hasPointB = false;

    if (!BoundsOverlap(GetWorldBounds(a), GetWorldBounds(b))) {
        return false;
    }

    Point2D worldPointA;
    Point2D worldPointB;
    float shortestDistB = std::numeric_limits<float>::max();
    float shortestDistA = std::numeric_limits<float>::max();
    bool hasCollision = false;
    
    for (const Shape& currentShapeA : GetWorldShapes(a)) {
        for (const Shape& currentShapeB : GetWorldShapes(b)) {
            
            Point2D currentContactPoint;
            
            if (currentShapeA.t == ShapeType::kLine && FirstContact(currentShapeA.shape.line, currentShapeB, currentContactPoint)) {
                hasCollision = true;
                float currentDistance = currentShapeA.shape.line.p1.Distance(currentContactPoint);
                if (currentDistance < shortestDistA) {
                    shortestDistA = currentDistance;
                    worldPointA =currentContactPoint;
                    hasPointA = true;
                }
            }

            if (currentShapeB.t == ShapeType::kLine && FirstContact(currentShapeB.shape.line, currentShapeA, currentContactPoint)) {
                hasCollision = true;
                float distance = currentShapeB.shape.line.p1.Distance(currentContactPoint);
                if (distance <shortestDistB) {
                    shortestDistB =distance;
                    worldPointB = currentContactPoint;
                    hasPointB = true;
                }
            }

            if (currentShapeA.t != ShapeType::kLine && currentShapeB.t != ShapeType::kLine && ShapesOverlap(currentShapeA, currentShapeB)) {
                hasCollision = true;
            }
        }
    }

    if (hasPointA) {
        SetupTransform(a);
        pointA = mContext.ScreenContext->ReverseTransform(worldPointA);
    }
    if (hasPointB) {
        SetupTransform(b);
        pointB = mContext.ScreenContext->ReverseTransform(worldPointB);
    }
    return hasCollision;
}


}  // namespace CMPUT350
