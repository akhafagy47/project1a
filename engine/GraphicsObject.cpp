#include "GraphicsObject.h"

namespace CMPUT350 {

// Default implementations draw nothing; derived objects override the layer(s) they use.
void GraphicsObject::RenderBackground(GameContext*) {}
void GraphicsObject::RenderForeground(GameContext*) {}

}  // namespace CMPUT350
