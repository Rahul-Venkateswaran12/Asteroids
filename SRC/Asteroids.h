#ifndef __ASTEROIDS_H__
#define __ASTEROIDS_H__

#include "GameUtil.h"
#include "GameSession.h"
#include "IKeyboardListener.h"
#include "IGameWorldListener.h"
#include "IScoreListener.h"
#include "ScoreKeeper.h"
#include "Player.h"
#include "IPlayerListener.h"
#include "GUIContainer.h"

class GameObject;
class Spaceship;
class GUILabel;

class Asteroids : public GameSession, public IKeyboardListener, public IGameWorldListener, public IScoreListener, public IPlayerListener
{
public:
    Asteroids(int argc, char* argv[]);
    virtual ~Asteroids(void);

    virtual void Start(void);
    virtual void Stop(void);

    // IKeyboardListener interface
    void OnKeyPressed(uchar key, int x, int y);
    void OnKeyReleased(uchar key, int x, int y);
    void OnSpecialKeyPressed(int key, int x, int y);
    void OnSpecialKeyReleased(int key, int x, int y);

    // IScoreListener interface
    void OnScoreChanged(int score);

    // IPlayerListener interface
    void OnPlayerKilled(int lives_left);

    // IGameWorldListener interface
    void OnWorldUpdated(GameWorld* world) {}
    void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) {}
    void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object);

    // ITimerListener interface
    void OnTimer(int value);

private:
    shared_ptr<Spaceship> mSpaceship;
    // Display labels
    shared_ptr<GUILabel> mScoreLabel;
    shared_ptr<GUILabel> mLivesLabel;
    shared_ptr<GUILabel> mGameOverLabel;
    shared_ptr<GUILabel> mStartGameLabel;
    shared_ptr<GUILabel> mDifficultyLabel;
    shared_ptr<GUILabel> mInstructionsLabel;
    shared_ptr<GUILabel> mHighScoresLabel;
    shared_ptr<GUILabel> mTitleLabel;
    shared_ptr<GUILabel> mInstructionsThrust;
    shared_ptr<GUILabel> mInstructionsDirection;
    shared_ptr<GUILabel> mInstructionsShoot;
    shared_ptr<GUILabel> mInstructionsGoal;
    shared_ptr<GUILabel> mInstructionsExit;

    uint mLevel;
    uint mAsteroidCount;
	// Game state variables
    bool mIsStartScreen;
    int mSelectedMenuOption;
    bool mShowingInstructions;

    const static uint SHOW_GAME_OVER = 0;
    const static uint START_NEXT_LEVEL = 1;
    const static uint CREATE_NEW_PLAYER = 2;

    ScoreKeeper mScoreKeeper;
    Player mPlayer;

    shared_ptr<GameObject> CreateSpaceship();
    void CreateAsteroids(const uint num_asteroids);
    void CreateGUI();
    shared_ptr<GameObject> CreateExplosion();
    void StartGame();
    void UpdateMenuDisplay();
};

#endif