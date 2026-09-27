#ifndef GRAPHICS_OBJECT_H
#define GRAPHICS_OBJECT_H

#include "GameObject.h"

namespace CMPUT350 {

class GameContext;

/**
 * @brief A game object that draws itself. Only graphics objects are rendered by the engine.
 *
 * Every background is drawn before any foreground, so objects can guarantee they appear behind
 * others regardless of the order objects are stored in the engine.
 */
class GraphicsObject : public GameObject {
public:
    /**
     * @brief Draws the parts of the object that should be behind all foreground drawing.
     * @param context Engine and drawing context for this frame.
     */
    virtual void RenderBackground(GameContext *context);

    /**
     * @brief Draws the parts of the object that should be in front of all background drawing.
     * @param context Engine and drawing context for this frame.
     */
    virtual void RenderForeground(GameContext *context);
};

}  // namespace CMPUT350

#endif
