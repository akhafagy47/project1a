#include "GraphicsObject.h"

namespace CMPUT350 {

// Default implementations draw nothing; derived objects override the layer(s) they use.
void GraphicsObject::RenderBackground(GameContext *context) {}
void GraphicsObject::RenderForeground(GameContext *context) {}

}  // namespace CMPUT350
