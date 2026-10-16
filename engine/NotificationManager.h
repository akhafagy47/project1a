#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace CMPUT350 {

class GameObject;

/**
 * @brief Delivers string messages to every object that has registered for them, so objects can
 * communicate without holding references to each other.
 *
 * Listeners are stored as weak pointers only, so registering never keeps an object alive. Entries
 * for objects that have been destroyed are removed the next time their list is used.
 */
class NotificationManager {
public:
    /**
     * @brief Registers an object to receive a message. Registering the same object for the same
     * message twice has no effect.
     * @param object The object to notify. Only a weak reference is kept.
     * @param key The message the object wants to receive.
     */
    void Register(std::weak_ptr<GameObject> object, const std::string& key);

    /**
     * @brief Stops an object from receiving a message. Also removes any listeners for that message
     * whose objects have been destroyed.
     * @param object The object to remove. It may already be destroyed.
     * @param key The message the object should no longer receive.
     */
    void Unregister(std::weak_ptr<GameObject> object, const std::string& key);

    /**
     * @brief Immediately calls ReceiveNotification(message) on every live object registered for
     * the message, then removes listeners whose objects have been destroyed.
     *
     * Listeners may safely call Register or Unregister (or Notify) from inside
     * ReceiveNotification: delivery uses a copy of the listener list, so a change made during
     * delivery applies from the next Notify call onward.
     * @param message The message to send.
     */
    void Notify(const std::string& message);

private:
    // Message -> objects registered for it. Weak pointers, so the manager never owns an object.
    std::unordered_map<std::string, std::vector<std::weak_ptr<GameObject>>> mListeners;
};

}  // namespace CMPUT350

#endif  // NOTIFICATIONMANAGER_H
