#include "GameObject.h"

namespace CMPUT350 {

// Default implementations do nothing; derived objects override what they need.
void GameObject::Initialize(GameContext*) {}
void GameObject::Update(GameContext*) {}
void GameObject::LateUpdate(GameContext*) {}
void GameObject::RenderUI(GameContext*) {}
void GameObject::ReceiveNotification(const std::string&) {}
bool GameObject::IsAlive() const { return mAlive; }
void GameObject::Kill() { mAlive = false; }
}  // namespace CMPUT350
