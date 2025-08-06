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

class GameObject;
class Spaceship;
class GUILabel;

class Asteroids : public GameSession, public IKeyboardListener, public IGameWorldListener, public IScoreListener, public IPlayerListener
{
public:
	// Enum to manage game states
	enum GameState {
		STATE_MAIN_MENU,
		STATE_PLAYING,
		STATE_DIFFICULTY_MENU,
		STATE_INSTRUCTIONS,
		STATE_HIGH_SCORES
	};

	Asteroids(int argc, char* argv[]);
	virtual ~Asteroids(void);

	virtual void Start(void);
	virtual void Stop(void);

	// Declaration of IKeyboardListener interface ////////////////////////////////
	void OnKeyPressed(uchar key, int x, int y);
	void OnKeyReleased(uchar key, int x, int y);
	void OnSpecialKeyPressed(int key, int x, int y);
	void OnSpecialKeyReleased(int key, int x, int y);

	// Declaration of IScoreListener interface //////////////////////////////////
	void OnScoreChanged(int score);

	// Declaration of the IPlayerLister interface //////////////////////////////
	void OnPlayerKilled(int lives_left);

	// Declaration of IGameWorldListener interface //////////////////////////////
	void OnWorldUpdated(GameWorld* world) {}
	void OnObjectAdded(GameWorld* world, shared_ptr<GameObject> object) {}
	void OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object);

	// Override the default implementation of ITimerListener ////////////////////
	void OnTimer(int value);

private:
	shared_ptr<Spaceship> mSpaceship;
	shared_ptr<GUILabel> mScoreLabel;
	shared_ptr<GUILabel> mLivesLabel;
	shared_ptr<GUILabel> mGameOverLabel;
	// Main menu labels
	shared_ptr<GUILabel> mTitleLabel;
	shared_ptr<GUILabel> mStartLabel;
	shared_ptr<GUILabel> mDifficultyLabel;
	shared_ptr<GUILabel> mInstructionsLabel;
	shared_ptr<GUILabel> mHighScoresLabel;
	// Difficulty submenu labels
	shared_ptr<GUILabel> mDifficultyTitleLabel;
	shared_ptr<GUILabel> mEasyLabel;
	shared_ptr<GUILabel> mNormalLabel;
	shared_ptr<GUILabel> mHardLabel;
	shared_ptr<GUILabel> mDifficultyExitLabel;
	// Instructions screen label
	shared_ptr<GUILabel> mInstructionsTextLabel;
	// High scores screen labels
	shared_ptr<GUILabel> mHighScoresTitleLabel;
	shared_ptr<GUILabel> mHighScoresTableLabel;
	shared_ptr<GUILabel> mHighScoresExitLabel;

	uint mLevel;
	uint mAsteroidCount;
	GameState mGameState;
	int mSelectedOption; // Tracks selected menu option
	float mPowerUpSpawnRate; // Stores difficulty setting

	void ResetSpaceship();
	shared_ptr<GameObject> CreateSpaceship();
	void CreateGUI();
	void CreateAsteroids(const uint num_asteroids);
	shared_ptr<GameObject> CreateExplosion();

	const static uint SHOW_GAME_OVER = 0;
	const static uint START_NEXT_LEVEL = 1;
	const static uint CREATE_NEW_PLAYER = 2;

	ScoreKeeper mScoreKeeper;
	Player mPlayer;
};

#endif