// TEMPORARY main for testing the engine and enemies until the Galaga class exists. It will be
// replaced by the spec's main, which only adds a Galaga object to the engine.
#include <memory>
#include <vector>

#include "Enemy.h"
#include "GameEngine.h"
#include "Player.h"
#include "Stars.h"

/**
 * @brief Test-only stand-in for Galaga's attack logic: every 90 frames, asks the next enemy in
 * formation to attack the player.
 */
class AttackDriver : public CMPUT350::GameObject
{
public:
    explicit AttackDriver(std::weak_ptr<Player> player) : mPlayer(std::move(player)) {}

    void Update(CMPUT350::GameContext* context) override
    {
        std::shared_ptr<Player> player = mPlayer.lock();
        if (player == nullptr || context->currentFrame % 90 != 0)
        {
            return;
        }

        // Walk the engine's objects starting after the last attacker, so attacks rotate
        std::vector<std::shared_ptr<Enemy>> enemies;
        for (const auto& object : *context->mEngineView)
        {
            if (auto enemy = std::dynamic_pointer_cast<Enemy>(object))
            {
                enemies.push_back(enemy);
            }
        }
        for (size_t i = 0; i < enemies.size(); i++)
        {
            mNext = (mNext + 1) % enemies.size();
            if (enemies[mNext]->Attack(context, player->GetLocation()))
            {
                return;
            }
        }
    }

private:
    std::weak_ptr<Player> mPlayer;
    size_t mNext = 0;
};

/**
 * @brief Builds a one-segment entry path from off-screen to a formation slot, curving through
 * the middle of the screen and rising into the slot from below.
 */
static std::vector<CMPUT350::Point2D> EntryPath(CMPUT350::Point2D home, bool fromLeft)
{
    CMPUT350::Point2D start(fromLeft ? -40.0f : 808.0f, 500.0f);
    return {start, CMPUT350::Point2D(384.0f, 800.0f), home + CMPUT350::Point2D(0.0f, 250.0f), home};
}

int main()
{
    CMPUT350::GameEngine engine(768, 1024, "Galaga");
    auto player = std::make_shared<Player>(CMPUT350::Point2D(384.0f, 900.0f));
    engine.AddGameObject(player);
    engine.AddGameObject(std::make_shared<Stars>(250, CMPUT350::Rect(0, 0, 768, 1024)));

    // Three rows of eight: bosses on top, then butterflies, then bees
    for (int row = 0; row < 3; row++)
    {
        for (int column = 0; column < 8; column++)
        {
            CMPUT350::Point2D home(104.0f + column * 80.0f, 100.0f + row * 70.0f);
            std::vector<CMPUT350::Point2D> path = EntryPath(home, column < 4);
            std::shared_ptr<Enemy> enemy;
            if (row == 0)
            {
                enemy = std::make_shared<Enemy3>(home, path);
            }
            else if (row == 1)
            {
                enemy = std::make_shared<Enemy2>(home, path);
            }
            else
            {
                enemy = std::make_shared<Enemy1>(home, path);
            }
            engine.AddGameObject(enemy);
        }
    }
    engine.AddGameObject(std::make_shared<AttackDriver>(player));

    engine.Run();
    return 0;
}
