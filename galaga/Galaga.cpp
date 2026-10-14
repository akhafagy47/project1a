#include "Galaga.h"

#include <algorithm>

#include "Bullet.h"
#include "Enemy.h"
#include "Keyboard.h"
#include "Player.h"
#include "Stars.h"

namespace
{
const int kStartingLives = 3;
const int kMaxCredits = 99;

const int kStartupFrames = 90;   // How long "STAGE n" is shown
const int kReadyFrames = 60;     // How long "READY" is shown before the player respawns
const int kGameOverFrames = 150; // How long "GAME OVER" is shown

const int kWaves = 5;
const int kShipsPerWave = 8;
const int kTotalEnemies = kWaves * kShipsPerWave;
const int kWaveInterval = 60;  // Frames between the first ships of consecutive waves
const int kShipSpacing = 7;    // Frames between ships in a wave

const int kFormationRows = 5;
const int kFormationColumns = 8;
const float kColumnSpacing = 70.0f;
const float kRowSpacing = 60.0f;
const float kFormationTop = 120.0f;

const int kFirstAttackInterval = 60;  // Frames between attack orders on stage 1
const int kMinAttackInterval = 20;
const int kAttackIntervalStep = 10;   // Each stage attacks this many frames sooner
const int kAttackStride = 7;          // Picks the (attackNumber * 7)th enemy, spreading attackers out

const float kPlayerBottomMargin = 124.0f;  // Player's distance from the bottom of the screen

/**
 * @brief Points for destroying each type of enemy, in formation and out of formation.
 */
struct EnemyPoints
{
    const char* typeName;
    int inFormation;
    int outOfFormation;
};

const EnemyPoints kEnemyPoints[] = {
    {"Enemy1", 50, 100},
    {"Enemy2", 80, 160},
    {"Enemy3", 150, 400},
};

const char* kInsertCoin = "InsertCoin";
const char* kStartGame = "StartGame";
const char* kPlayerDied = "PlayerDied";
const char* kKilledInFormation = "KilledInFormation";
const char* kKilledOutOfFormation = "KilledOutOfFormation";
}  // namespace

Galaga::Galaga(int width, int height) : mWidth(width), mHeight(height) {}

void Galaga::Initialize(CMPUT350::GameContext* context)
{
    CMPUT350::EngineView* engine = context->mEngineView;
    engine->InstallKeyDownNotification(CMPUT350::Key::Num5, kInsertCoin);
    engine->InstallKeyDownNotification(CMPUT350::Key::Num1, kStartGame);
    for (unsigned int key : {CMPUT350::Key::A, CMPUT350::Key::Left})
    {
        engine->InstallKeyDownNotification(key, kMoveLeft);
        engine->InstallKeyUpNotification(key, kStopLeft);
    }
    for (unsigned int key : {CMPUT350::Key::D, CMPUT350::Key::Right})
    {
        engine->InstallKeyDownNotification(key, kMoveRight);
        engine->InstallKeyUpNotification(key, kStopRight);
    }
    engine->InstallKeyDownNotification(CMPUT350::Key::Space, kFire);

    CMPUT350::NotificationManager* notifications = context->mNotificationManager;
    notifications->Register(context->CurrObject, kInsertCoin);
    notifications->Register(context->CurrObject, kStartGame);
    notifications->Register(context->CurrObject, kPlayerDied);
    for (const EnemyPoints& enemy : kEnemyPoints)
    {
        notifications->Register(context->CurrObject, std::string(enemy.typeName) + kKilledInFormation);
        notifications->Register(context->CurrObject,
                                std::string(enemy.typeName) + kKilledOutOfFormation);
    }

    engine->AddGameObject(std::make_shared<Stars>(250, CMPUT350::Rect(0, 0, mWidth, mHeight)));
}

void Galaga::Update(CMPUT350::GameContext* context)
{
    if (mStartRequested)
    {
        mStartRequested = false;
        if (mState == State::WaitingForCoin && mCredits > 0)
        {
            mCredits--;
            mScore = 0;
            mLives = kStartingLives;
            mStage = 1;
            mHasPlayer = false;
            mPlayerDied = false;
            ChangeState(State::Startup, context);
        }
    }

    // The player can be hit in any state where it is on screen
    bool inPlay = mState == State::Startup || mState == State::Waves || mState == State::Playing;
    if (inPlay && mHasPlayer && (mPlayerDied || !PlayerAlive()))
    {
        HandlePlayerDied(context);
    }

    switch (mState)
    {
    case State::WaitingForCoin:
        break;
    case State::Startup:
        UpdateStartup(context);
        break;
    case State::Waves:
        UpdateWaves(context);
        break;
    case State::Playing:
        UpdatePlaying(context);
        break;
    case State::PlayerDown:
        UpdatePlayerDown(context);
        break;
    case State::GameOver:
        UpdateGameOver(context);
        break;
    }
    mStateFrames++;
}

void Galaga::RenderUI(CMPUT350::GameContext* context)
{
    CMPUT350::DrawContext* gui = context->GUIContext;
    float centerX = mWidth / 2.0f;
    CMPUT350::Point2D center(centerX, mHeight / 2.0f);

    gui->DrawCenteredText("HIGH SCORE", 24, CMPUT350::Point2D(centerX, 20.0f), CMPUT350::Colors::red);
    gui->DrawCenteredText(std::to_string(mHighScore), 24, CMPUT350::Point2D(centerX, 48.0f),
                          CMPUT350::Colors::white);

    if (mState == State::WaitingForCoin)
    {
        gui->DrawCenteredText("GALAGA", 72, CMPUT350::Point2D(centerX, 300.0f), CMPUT350::Colors::cyan);
        gui->DrawCenteredText("PRESS 5 TO INSERT COIN", 28, center, CMPUT350::Colors::white);
        if (mCredits > 0)
        {
            gui->DrawCenteredText("PRESS 1 TO START", 28, center + CMPUT350::Point2D(0.0f, 50.0f),
                                  CMPUT350::Colors::yellow);
        }
        gui->DrawText("CREDITS " + std::to_string(mCredits), 24,
                      CMPUT350::Point2D(20.0f, mHeight - 40.0f), CMPUT350::Colors::white);
        return;
    }

    // In a game: score, lives and stage are always shown
    gui->DrawText("1UP", 24, CMPUT350::Point2D(20.0f, 8.0f), CMPUT350::Colors::red);
    gui->DrawText(std::to_string(mScore), 24, CMPUT350::Point2D(20.0f, 36.0f), CMPUT350::Colors::white);
    gui->DrawText("LIVES " + std::to_string(mLives), 24, CMPUT350::Point2D(20.0f, mHeight - 40.0f),
                  CMPUT350::Colors::white);
    gui->DrawText("STAGE " + std::to_string(mStage), 24,
                  CMPUT350::Point2D(mWidth - 150.0f, mHeight - 40.0f), CMPUT350::Colors::white);

    switch (mState)
    {
    case State::Startup:
        gui->DrawCenteredText("STAGE " + std::to_string(mStage), 36, center, CMPUT350::Colors::cyan);
        break;
    case State::PlayerDown:
        if (mReadyFrames >= 0)
        {
            gui->DrawCenteredText("READY", 36, center, CMPUT350::Colors::red);
            gui->DrawCenteredText("LIVES " + std::to_string(mLives), 28,
                                  center + CMPUT350::Point2D(0.0f, 50.0f), CMPUT350::Colors::white);
        }
        break;
    case State::GameOver:
        gui->DrawCenteredText("GAME OVER", 48, center, CMPUT350::Colors::red);
        break;
    default:
        break;
    }
}

void Galaga::ReceiveNotification(const std::string& key)
{
    if (key == kInsertCoin)
    {
        mCredits = std::min(mCredits + 1, kMaxCredits);
        return;
    }
    if (key == kStartGame)
    {
        mStartRequested = true;
        return;
    }
    if (key == kPlayerDied)
    {
        mPlayerDied = true;
        return;
    }

    for (const EnemyPoints& enemy : kEnemyPoints)
    {
        std::string typeName = enemy.typeName;
        if (key == typeName + kKilledInFormation || key == typeName + kKilledOutOfFormation)
        {
            bool inFormation = key == typeName + kKilledInFormation;
            AddPoints(inFormation ? enemy.inFormation : enemy.outOfFormation);
            mEnemiesAlive = std::max(mEnemiesAlive - 1, 0);
            return;
        }
    }
}

/**
 * @brief Switches state, restarting the state's frame counter. The player is paused while the
 * stage banner or the respawn message is shown.
 */
void Galaga::ChangeState(State next, CMPUT350::GameContext* context)
{
    bool wasPaused = mState == State::Startup || mState == State::PlayerDown;
    bool paused = next == State::Startup || next == State::PlayerDown;
    if (wasPaused && !paused)
    {
        context->mNotificationManager->Notify(kResumePlayer);
    }
    else if (!wasPaused && paused)
    {
        context->mNotificationManager->Notify(kPausePlayer);
    }

    switch (next)
    {
    case State::Startup:
        // A new stage: the formation is rebuilt from scratch
        mSpawned = 0;
        mWaveFrames = 0;
        mEnemiesAlive = 0;
        mAttackNumber = 0;
        break;
    case State::PlayerDown:
        mReadyFrames = -1;
        break;
    case State::GameOver:
        mHighScore = std::max(mHighScore, mScore);
        break;
    default:
        break;
    }

    mState = next;
    mStateFrames = 0;
}

/**
 * @brief Shows the stage number, brings in the player if there isn't one, then starts the waves.
 */
void Galaga::UpdateStartup(CMPUT350::GameContext* context)
{
    if (!mHasPlayer)
    {
        SpawnPlayer(context);
    }
    if (mStateFrames >= kStartupFrames)
    {
        ChangeState(State::Waves, context);
    }
}

/**
 * @brief Spawns the enemies on their schedule, then starts play once all of them have reached
 * the formation.
 */
void Galaga::UpdateWaves(CMPUT350::GameContext* context)
{
    // Ship i of wave w enters w * kWaveInterval + i * kShipSpacing frames into the stage
    while (mSpawned < kTotalEnemies)
    {
        int wave = mSpawned / kShipsPerWave;
        int ship = mSpawned % kShipsPerWave;
        if (wave * kWaveInterval + ship * kShipSpacing > mWaveFrames)
        {
            break;
        }
        SpawnEnemy(mSpawned, context);
        mSpawned++;
    }
    mWaveFrames++;

    if (mSpawned == kTotalEnemies && AllEnemiesInFormation(context))
    {
        ChangeState(State::Playing, context);
    }
}

/**
 * @brief Orders attacks at a regular interval and moves to the next stage once every enemy has
 * been destroyed.
 */
void Galaga::UpdatePlaying(CMPUT350::GameContext* context)
{
    if (mEnemiesAlive == 0)
    {
        mStage++;
        ChangeState(State::Startup, context);
        return;
    }

    int interval = std::max(kMinAttackInterval,
                            kFirstAttackInterval - kAttackIntervalStep * (mStage - 1));
    if (mStateFrames > 0 && mStateFrames % interval == 0)
    {
        OrderAttack(context);
    }
}

/**
 * @brief Waits for every enemy to return to formation, shows "READY" and the lives left, then
 * respawns the player and continues from where the game was.
 */
void Galaga::UpdatePlayerDown(CMPUT350::GameContext* context)
{
    if (mReadyFrames < 0)
    {
        if (AllEnemiesInFormation(context))
        {
            mReadyFrames = 0;
        }
        return;
    }

    mReadyFrames++;
    if (mReadyFrames < kReadyFrames)
    {
        return;
    }

    if (mResumeState != State::Startup)
    {
        SpawnPlayer(context);  // Startup spawns the player itself
    }
    ChangeState(mResumeState, context);
}

/**
 * @brief Shows "GAME OVER", then clears the screen and waits for the next game.
 */
void Galaga::UpdateGameOver(CMPUT350::GameContext* context)
{
    if (mStateFrames >= kGameOverFrames)
    {
        KillEnemiesAndBullets(context);
        ChangeState(State::WaitingForCoin, context);
    }
}

/**
 * @brief Adds a new player at the bottom center of the screen.
 */
void Galaga::SpawnPlayer(CMPUT350::GameContext* context)
{
    auto player = std::make_shared<Player>(
        CMPUT350::Point2D(mWidth / 2.0f, mHeight - kPlayerBottomMargin));
    mPlayer = player;
    mHasPlayer = true;
    mPlayerDied = false;
    context->mEngineView->AddGameObject(player);
}

/**
 * @brief Adds the index-th enemy of the stage (in spawn order).
 *
 * Wave w takes slots w, w + 5, w + 10, ... of the 40 formation slots (numbered row by row), so
 * every wave fills parts of all five rows and brings in all three enemy types. Row 0 holds
 * Enemy3, rows 1-2 Enemy2 and rows 3-4 Enemy1.
 */
void Galaga::SpawnEnemy(int index, CMPUT350::GameContext* context)
{
    int wave = index / kShipsPerWave;
    int ship = index % kShipsPerWave;
    int slot = ship * kWaves + wave;
    int row = slot / kFormationColumns;
    int column = slot % kFormationColumns;

    CMPUT350::Point2D home(mWidth / 2.0f + (column - (kFormationColumns - 1) / 2.0f) * kColumnSpacing,
                           kFormationTop + row * kRowSpacing);
    std::vector<CMPUT350::Point2D> path = EntryPath(wave, home);

    std::shared_ptr<Enemy> enemy;
    if (row == 0)
    {
        enemy = std::make_shared<Enemy3>(home, path);
    }
    else if (row < 3)
    {
        enemy = std::make_shared<Enemy2>(home, path);
    }
    else
    {
        enemy = std::make_shared<Enemy1>(home, path);
    }
    context->mEngineView->AddGameObject(enemy);
    mEnemiesAlive++;
}

/**
 * @brief Builds a two-segment entry path (7 control points) ending at the given slot.
 *
 * Even waves come in from the left and odd waves from the right (mirrored). The ship flies in
 * from the side, swoops down across the lower screen, then rises into its slot from below. The
 * middle control points are collinear so the two segments join smoothly.
 */
std::vector<CMPUT350::Point2D> Galaga::EntryPath(int wave, CMPUT350::Point2D slot) const
{
    bool fromLeft = wave % 2 == 0;
    float width = static_cast<float>(mWidth);
    auto side = [fromLeft, width](float x, float y)
    { return CMPUT350::Point2D(fromLeft ? x : width - x, y); };

    return {side(-40.0f, 260.0f),  side(240.0f, 260.0f), side(120.0f, 680.0f),
            side(330.0f, 680.0f),  side(480.0f, 680.0f), slot + CMPUT350::Point2D(0.0f, 260.0f),
            slot};
}

/**
 * @brief Whether the current player is still alive.
 */
bool Galaga::PlayerAlive() const
{
    std::shared_ptr<Player> player = mPlayer.lock();
    return player != nullptr && player->IsAlive();
}

/**
 * @brief Whether every live enemy is in its formation slot. Enemies added this frame are not in
 * the engine's list yet, so the number found must also match the number spawned and alive.
 */
bool Galaga::AllEnemiesInFormation(CMPUT350::GameContext* context) const
{
    int found = 0;
    for (const auto& object : *context->mEngineView)
    {
        auto enemy = std::dynamic_pointer_cast<Enemy>(object);
        if (enemy == nullptr || !enemy->IsAlive())
        {
            continue;
        }
        if (!enemy->IsInFormation())
        {
            return false;
        }
        found++;
    }
    return found == mEnemiesAlive;
}

/**
 * @brief Picks an enemy deterministically and tells it to attack the player. The k-th live enemy
 * is chosen with k = (attackNumber * 7) mod count, so consecutive attackers are spread across
 * the formation. An enemy that is not in formation declines and this turn is skipped.
 */
void Galaga::OrderAttack(CMPUT350::GameContext* context)
{
    std::shared_ptr<Player> player = mPlayer.lock();
    if (player == nullptr || !player->IsAlive())
    {
        return;
    }

    std::vector<std::shared_ptr<Enemy>> enemies;
    for (const auto& object : *context->mEngineView)
    {
        auto enemy = std::dynamic_pointer_cast<Enemy>(object);
        if (enemy != nullptr && enemy->IsAlive())
        {
            enemies.push_back(enemy);
        }
    }
    if (enemies.empty())
    {
        return;
    }

    size_t k = static_cast<size_t>(mAttackNumber) * kAttackStride % enemies.size();
    mAttackNumber++;
    enemies[k]->Attack(context, player->GetLocation());
}

/**
 * @brief Takes a life and waits for the formation before respawning, or ends the game if no
 * lives are left.
 */
void Galaga::HandlePlayerDied(CMPUT350::GameContext* context)
{
    mHasPlayer = false;
    mPlayerDied = false;
    mLives--;
    if (mLives <= 0)
    {
        mLives = 0;
        ChangeState(State::GameOver, context);
        return;
    }

    // Dying during the stage banner restarts the banner; otherwise continue where the game was
    mResumeState = mState;
    ChangeState(State::PlayerDown, context);
}

/**
 * @brief Removes every enemy and bullet so the next game starts from an empty screen.
 */
void Galaga::KillEnemiesAndBullets(CMPUT350::GameContext* context)
{
    for (const auto& object : *context->mEngineView)
    {
        if (std::dynamic_pointer_cast<Enemy>(object) != nullptr ||
            std::dynamic_pointer_cast<Bullet>(object) != nullptr)
        {
            object->Kill();
        }
    }
    mEnemiesAlive = 0;
}

/**
 * @brief Adds to the score, raising the high score along with it.
 */
void Galaga::AddPoints(int points)
{
    mScore += points;
    mHighScore = std::max(mHighScore, mScore);
}
