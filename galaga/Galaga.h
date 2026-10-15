#ifndef GALAGA_H
#define GALAGA_H

#include <memory>
#include <string>
#include <vector>

#include "GameContext.h"
#include "GameObject.h"
#include "MathUtil.h"

class Player;

/**
 * @brief The game manager: runs the game's state machine, creates and removes every other game
 * object, keeps score and draws the GUI.
 *
 * State flow:
 *   WaitingForCoin -(1 with credits)-> Startup -> Waves -> Playing
 *   Playing -(all enemies destroyed)-> Startup (next stage)
 *   Startup/Waves/Playing -(player died)-> PlayerDown -> back to where it was, or GameOver
 *   GameOver -> WaitingForCoin
 *
 * Galaga is not a graphics object; everything it shows is drawn in RenderUI() through the GUI
 * context, so nothing is offset or rotated.
 */
class Galaga : public CMPUT350::GameObject
{
public:
    /**
     * @brief Constructs the game manager.
     * @param width Width of the play area in pixels.
     * @param height Height of the play area in pixels.
     */
    Galaga(int width, int height);

    /**
     * @brief Installs the key notifications, registers for the game's notifications and adds the
     * star background.
     * @param context Pointer to global GameContext.
     */
    void Initialize(CMPUT350::GameContext* context) override;

    /**
     * @brief Advances the state machine by one frame: spawns enemies and the player, orders
     * attacks and detects the player's death.
     * @param context Pointer to global GameContext.
     */
    void Update(CMPUT350::GameContext* context) override;

    /**
     * @brief Draws the score, high score, lives, stage and the current state's message.
     * @param context Pointer to global GameContext.
     */
    void RenderUI(CMPUT350::GameContext* context) override;

    /**
     * @brief Handles coins, the start button, the player's death and enemy kills (which update
     * the score and the count of live enemies).
     * @param key The notification's message.
     */
    void ReceiveNotification(const std::string& key) override;

private:
    enum class State
    {
        WaitingForCoin,
        Startup,
        Waves,
        Playing,
        PlayerDown,
        GameOver
    };

    void ChangeState(State next);
    void UpdateStartup(CMPUT350::GameContext* context);
    void UpdateWaves(CMPUT350::GameContext* context);
    void UpdatePlaying(CMPUT350::GameContext* context);
    void UpdatePlayerDown(CMPUT350::GameContext* context);
    void UpdateGameOver(CMPUT350::GameContext* context);

    void SpawnPlayer(CMPUT350::GameContext* context);
    void SpawnEnemy(int index, CMPUT350::GameContext* context);
    std::vector<CMPUT350::Point2D> EntryPath(int wave, CMPUT350::Point2D slot) const;
    bool PlayerAlive() const;
    bool AllEnemiesInFormation(CMPUT350::GameContext* context) const;
    void OrderAttack(CMPUT350::GameContext* context);
    void HandlePlayerDied();
    void KillEnemiesAndBullets(CMPUT350::GameContext* context);
    void AddPoints(int points);

    int mWidth;
    int mHeight;

    State mState = State::WaitingForCoin;
    int mStateFrames = 0;  // Frames spent in the current state

    int mCredits = 0;
    int mScore = 0;
    int mHighScore = 0;
    int mLives = 0;
    int mStage = 0;

    int mSpawned = 0;       // Enemies spawned so far this stage
    int mEnemiesAlive = 0;  // Spawned enemies that have not been destroyed
    int mWaveFrames = 0;    // Frames spent spawning this stage (paused while the player is down)
    int mAttackNumber = 0;

    // Notifications arrive without a GameContext, so these are acted on in the next Update()
    bool mStartRequested = false;  // StartGame received
    bool mPlayerDied = false;      // PlayerDied received

    bool mHasPlayer = false;  // A player has been spawned and has not died yet
    State mResumeState = State::Playing;  // Where to go after the player respawns
    int mReadyFrames = -1;  // Frames showing "READY", or -1 while waiting for the formation

    std::weak_ptr<Player> mPlayer;
};

#endif  // GALAGA_H
