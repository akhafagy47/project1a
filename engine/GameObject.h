#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>
namespace CMPUT350 {

class GameContext;

/**
 * @brief Base class for every object that can be part of the game.
 *
 * All methods have default (empty) implementations so derived objects only override what they
 * need. By default an object is alive until Kill() is called.
 */
class GameObject {
public:
    virtual ~GameObject() = default;

    /**
     * @brief Called once, at the start of the frame in which the object enters the engine.
     * @param context Engine and drawing context for this frame.
     */
    virtual void Initialize(GameContext *context);

    /**
     * @brief Called once per frame, before collisions are processed.
     * @param context Engine and drawing context for this frame.
     */
    virtual void Update(GameContext *context);

    /**
     * @brief Called once per frame, after collisions are processed.
     * @param context Engine and drawing context for this frame.
     */
    virtual void LateUpdate(GameContext *context);

    /**
     * @brief Called once per frame after all background and foreground rendering, so UI is drawn
     * on top of everything else.
     * @param context Engine and drawing context for this frame.
     */
    virtual void RenderUI(GameContext *context);

    

    /**
     * @brief Whether the object should stay in the engine.
     * @return false if the object should be removed at the start of the next frame.
     */
    virtual bool IsAlive() const;

    /**
     * @brief Marks the object as dead so the engine removes it at the start of the next frame.
     */
    virtual void Kill();

    virtual void ReceiveNotification(const std::string &key);

private:
    bool mAlive = true;
};

}  // namespace CMPUT350

#endif  // GAMEOBJECT_H
