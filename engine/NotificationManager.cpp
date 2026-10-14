#include "NotificationManager.h"

#include <algorithm>

#include "GameObject.h"

namespace CMPUT350 {

/**
 * @brief Returns whether two weak pointers refer to the same object. owner_before compares the
 * pointers' control blocks, so this still works after the object has been destroyed.
 */
static bool SameObject(const std::weak_ptr<GameObject> &a, const std::weak_ptr<GameObject> &b) {
    return !a.owner_before(b) && !b.owner_before(a);
}

/**
 * @brief Removes every entry in a listener list whose object has been destroyed.
 */
static void RemoveExpired(std::vector<std::weak_ptr<GameObject>> &listeners) {
    std::erase_if(listeners, [](const std::weak_ptr<GameObject> &l) { return l.expired(); });
}

void NotificationManager::Register(std::weak_ptr<GameObject> object, const std::string &key) {
    std::vector<std::weak_ptr<GameObject>> &listeners = mListeners[key];
    for (const auto &listener : listeners) {
        if (SameObject(listener, object)) {
            return;  // Already registered for this message
        }
    }
    listeners.push_back(std::move(object));
}

void NotificationManager::Unregister(std::weak_ptr<GameObject> object, const std::string &key) {
    auto it = mListeners.find(key);
    if (it == mListeners.end()) {
        return;  // Nothing registered for this message
    }

    std::vector<std::weak_ptr<GameObject>> &listeners = it->second;
    std::erase_if(listeners, [&object](const std::weak_ptr<GameObject> &l) {
        return SameObject(l, object) || l.expired();
    });
    if (listeners.empty()) {
        mListeners.erase(it);
    }
}

void NotificationManager::Notify(const std::string &message) {
    auto it = mListeners.find(message);
    if (it == mListeners.end()) {
        return;  // Nobody is listening
    }

    // Deliver from a copy: a listener may Register/Unregister during ReceiveNotification, which
    // would invalidate iteration over the real list.
    std::vector<std::weak_ptr<GameObject>> listeners = it->second;
    for (const auto &listener : listeners) {
        if (std::shared_ptr<GameObject> object = listener.lock()) {
            object->ReceiveNotification(message);
        }
    }

    // Look the list up again: callbacks may have changed the map, invalidating the iterator.
    it = mListeners.find(message);
    if (it != mListeners.end()) {
        RemoveExpired(it->second);
        if (it->second.empty()) {
            mListeners.erase(it);
        }
    }
}

}  // namespace CMPUT350
