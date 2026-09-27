#ifndef ENGINEVIEW_H
#define ENGINEVIEW_H

#include <memory>

namespace CMPUT350 {

class GameObject;

/**
 * @brief Restricted view of the game engine that is handed to game objects.
 *
 * Game objects should not be able to call engine functions such as Run(). This pure virtual
 * interface exposes only the operations that objects are allowed to perform on the engine.
 */
class EngineView {
public:
    virtual ~EngineView() = default;

    /**
     * @brief Queues a new object to be added to the engine at the start of the next frame.
     * @param gameObject The object to add. The engine shares ownership of it.
     */
    virtual void AddGameObject(std::shared_ptr<GameObject> gameObject) = 0;
};

}  // namespace CMPUT350

#endif
