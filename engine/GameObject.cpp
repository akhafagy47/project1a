#include "GameObject.h"

namespace CMPUT350 {

// Default implementations do nothing; derived objects override what they need.
void GameObject::Initialize(GameContext *context) {}
void GameObject::Update(GameContext *context) {}
void GameObject::LateUpdate(GameContext *context) {}
void GameObject::RenderUI(GameContext *context) {}
bool GameObject::HandleKeyEvent(GameContext *context, char key) { return false; }
bool GameObject::IsAlive() const { return mAlive; }
void GameObject::Kill() { mAlive = false; }
}  // namespace CMPUT350
