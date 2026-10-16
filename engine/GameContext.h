#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H

#include <memory>

#include "DrawContext.h"
#include "EngineView.h"
#include "GameObject.h"
#include "NotificationManager.h"

namespace CMPUT350 {

/**
 * @brief Everything a game object may need from the engine, passed in on every engine call.
 *
 * The pointers are owned by the engine and are guaranteed to be valid for the duration of any
 * call that receives the context. They are raw pointers because game objects neither own nor
 * share them, and must not store them beyond the call.
 */
class GameContext {
public:
    EngineView* mEngineView = nullptr;     // Lets objects add new objects to the engine
    DrawContext* ScreenContext = nullptr;  // Lets graphics objects draw to the screen
    DrawContext* GUIContext = nullptr;     // Draws in screen space, with no transform
    std::weak_ptr<GameObject> CurrObject;  // The object the engine is currently calling
    NotificationManager* mNotificationManager = nullptr;  // Sends messages between objects
    int currentFrame = 0;                                 // Global frame counter
};

}  // namespace CMPUT350

#endif  // GAMECONTEXT_H
