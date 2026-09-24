#ifndef GAMECONTEXT_H
#define GAMECONTEXT_H

#include "DrawContext.h"
#include "EngineView.h"
#include "GameObject.h"

namespace CMPUT350 {

/**
 * @brief Everything a game object may need from the engine, passed in on every engine call.
 *
 * Both pointers are owned by the engine and are guaranteed to be valid for the duration of any
 * call that receives the context. They are raw pointers because game objects neither own nor
 * share them, and must not store them beyond the call.
 */
class GameContext {
public:
    EngineView *mEngineView = nullptr;     // Lets objects add new objects to the engine
    DrawContext *ScreenContext = nullptr;  // Lets graphics objects draw to the screen
};

}  // namespace CMPUT350

#endif  // GAMECONTEXT_H
