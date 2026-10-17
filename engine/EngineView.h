#ifndef ENGINEVIEW_H
#define ENGINEVIEW_H

#include <memory>
#include <string>
#include <vector>

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

    using tGameObject = std::vector<std::shared_ptr<GameObject>>;

    /**
     * @brief Sends the notification to the notification manager whenever the key is pressed.
     * @param key The key's scancode.
     * @param notification The message to send.
     */
    virtual void InstallKeyDownNotification(int key, const std::string& notification) = 0;

    /**
     * @brief Sends the notification to the notification manager whenever the key is released.
     * @param key The key's scancode.
     * @param notification The message to send.
     */
    virtual void InstallKeyUpNotification(int key, const std::string& notification) = 0;

    using const_iterator = tGameObject::const_iterator;

    /**
     * @brief Iterators over every object currently in the engine, so objects can be looped over
     * with a range-based for loop.
     */
    virtual const_iterator cbegin() const = 0;
    virtual const_iterator cend() const = 0;
    virtual const_iterator begin() const = 0;
    virtual const_iterator end() const = 0;
};

}  // namespace CMPUT350

#endif
