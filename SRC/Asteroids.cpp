#include "Asteroid.h"
#include "Asteroids.h"
#include "Animation.h"
#include "AnimationManager.h"
#include "GameUtil.h"
#include "GameWindow.h"
#include "GameWorld.h"
#include "GameDisplay.h"
#include "Spaceship.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include "GUILabel.h"
#include "Explosion.h"

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Constructor. Takes arguments from command line, just in case. */
Asteroids::Asteroids(int argc, char* argv[])
	: GameSession(argc, argv), mGameState(STATE_MAIN_MENU), mSelectedOption(0), mPowerUpSpawnRate(1.0f)
{
	mLevel = 0;
	mAsteroidCount = 0;
}

/** Destructor. */
Asteroids::~Asteroids(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Start an asteroids game. */
void Asteroids::Start()
{
	// Create a shared pointer for the Asteroids game object - DO NOT REMOVE
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);

	// Add this class as a listener of the game world
	mGameWorld->AddListener(thisPtr.get());

	// Add this as a listener to the world and the keyboard
	mGameWindow->AddKeyboardListener(thisPtr);

	// Add a score keeper to the game world
	mGameWorld->AddListener(&mScoreKeeper);

	// Add this class as a listener of the score keeper
	mScoreKeeper.AddListener(thisPtr);

	// Create an ambient light to show sprite textures
	GLfloat ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glEnable(GL_LIGHT0);

	Animation* explosion_anim = AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
	Animation* asteroid1_anim = AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1", 128, 8192, 128, 128, "asteroid1_fs.png");
	Animation* spaceship_anim = AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 128, 128, 128, 128, "spaceship_fs.png");

	// Create asteroids for the start screen background
	CreateAsteroids(10);

	// Create the GUI
	CreateGUI();

	// Add a player (watcher) to the game world
	mGameWorld->AddListener(&mPlayer);

	// Add this class as a listener of the player
	mPlayer.AddListener(thisPtr);

	// Start the game
	GameSession::Start();
}

/** Stop the current game. */
void Asteroids::Stop()
{
	// Stop the game
	GameSession::Stop();
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IKeyboardListener /////////////////////

void Asteroids::OnKeyPressed(uchar key, int x, int y)
{
	if (mGameState == STATE_MAIN_MENU) {
		if (key == 13) { // Enter key
			switch (mSelectedOption) {
			case 0: // Start
				mGameState = STATE_PLAYING;
				// Hide main menu labels
				mTitleLabel->SetVisible(false);
				mStartLabel->SetVisible(false);
				mDifficultyLabel->SetVisible(false);
				mInstructionsLabel->SetVisible(false);
				mHighScoresLabel->SetVisible(false);
				// Show lives and score labels
				mLivesLabel->SetVisible(true);
				mScoreLabel->SetVisible(true);
				// Create and add spaceship
				mGameWorld->AddObject(CreateSpaceship());
				break;
			case 1: // Difficulty
				mGameState = STATE_DIFFICULTY_MENU;
				mSelectedOption = 0; // Reset to first difficulty option
				// Hide main menu labels
				mTitleLabel->SetVisible(false);
				mStartLabel->SetVisible(false);
				mDifficultyLabel->SetVisible(false);
				mInstructionsLabel->SetVisible(false);
				mHighScoresLabel->SetVisible(false);
				// Show difficulty menu labels
				mDifficultyTitleLabel->SetVisible(true);
				mEasyLabel->SetVisible(true);
				mNormalLabel->SetVisible(true);
				mHardLabel->SetVisible(true);
				mDifficultyExitLabel->SetVisible(true);
				// Highlight selected option
				mEasyLabel->SetColor(GLVector3f(1.0f, 1.0f, 0.0f)); // Yellow
				mNormalLabel->SetColor(GLVector3f(1.0f, 1.0f, 1.0f)); // White
				mHardLabel->SetColor(GLVector3f(1.0f, 1.0f, 1.0f)); // White
				break;
			case 2: // Instructions
				mGameState = STATE_INSTRUCTIONS;
				// Hide main menu labels
				mTitleLabel->SetVisible(false);
				mStartLabel->SetVisible(false);
				mDifficultyLabel->SetVisible(false);
				mInstructionsLabel->SetVisible(false);
				mHighScoresLabel->SetVisible(false);
				// Show instructions
				mInstructionsTextLabel->SetVisible(true);
				break;
			case 3: // High Scores
				mGameState = STATE_HIGH_SCORES;
				// Hide main menu labels
				mTitleLabel->SetVisible(false);
				mStartLabel->SetVisible(false);
				mDifficultyLabel->SetVisible(false);
				mInstructionsLabel->SetVisible(false);
				mHighScoresLabel->SetVisible(false);
				// Show high scores
				mHighScoresTitleLabel->SetVisible(true);
				mHighScoresTableLabel->SetVisible(true);
				mHighScoresExitLabel->SetVisible(true);
				break;
			}
		}
	}
	else if (mGameState == STATE_DIFFICULTY_MENU) {
		if (key == 13) { // Enter key
			switch (mSelectedOption) {
			case 0: // Easy
				mPowerUpSpawnRate = 1.5f;
				break;
			case 1: // Normal
				mPowerUpSpawnRate = 1.0f;
				break;
			case 2: // Hard
				mPowerUpSpawnRate = 0.0f;
				break;
			}
			// Return to main menu
			mGameState = STATE_MAIN_MENU;
			mSelectedOption = 1; // Return to Difficulty option
			// Hide difficulty menu labels
			mDifficultyTitleLabel->SetVisible(false);
			mEasyLabel->SetVisible(false);
			mNormalLabel->SetVisible(false);
			mHardLabel->SetVisible(false);
			mDifficultyExitLabel->SetVisible(false);
			// Show main menu labels
			mTitleLabel->SetVisible(true);
			mStartLabel->SetVisible(true);
			mDifficultyLabel->SetVisible(true);
			mInstructionsLabel->SetVisible(true);
			mHighScoresLabel->SetVisible(true);
			// Highlight selected option
			mStartLabel->SetColor(GLVector3f(1.0f, 1.0f, 1.0f)); // White
			mDifficultyLabel->SetColor(GLVector3f(1.0f, 1.0f, 0.0f)); // Yellow
			mInstructionsLabel->SetColor(GLVector3f(1.0f, 1.0f, 1.0f)); // White
			mHighScoresLabel->SetColor(GLVector3f(1.0f, 1.0f, 1.0f)); // White
		}
		else if (key == ' ') { // Spacebar to exit
			mGameState = STATE_MAIN_MENU;
			mSelectedOption = 1; // Return to Difficulty option
			// Hide difficulty menu labels
			mDifficultyTitleLabel->SetVisible(false);
			mEasyLabel->SetVisible(false);
			mNormalLabel->SetVisible(false);
			mHardLabel->SetVisible(false);
			mDifficultyExitLabel->SetVisible(false);
			// Show main menu labels
			mTitleLabel->SetVisible(true);
			mStartLabel->SetVisible(true);
			mDifficultyLabel->SetVisible(true);
			mInstructionsLabel->SetVisible(true);
			mHighScoresLabel->SetVisible(true);
			// Highlight selected option
			mStartLabel->SetColor(GLVector3f(1.0f, 1.0f, 1.0f)); // White
			mDifficultyLabel->SetColor(GLVector3f(1.0f, 1.0f, 0.0f)); // Yellow
			mInstructionsLabel->SetColor(GLVector3f(1.0f, 1.0f, 1.0f)); // White
			mHighScoresLabel->SetColor(GLVector3f(1.0f, 1.0f, 1.0f)); // White
		}
	}
	else if (mGameState == STATE_INSTRUCTIONS || mGameState == STATE_HIGH_SCORES) {
		if (key == ' ') { // Spacebar to exit
			mGameState = STATE_MAIN_MENU;
			mSelectedOption = mGameState == STATE_INSTRUCTIONS ? 2 : 3; // Return to respective option
			// Hide instructions or high scores labels
			mInstructionsTextLabel->SetVisible(false);
			mHighScoresTitleLabel->SetVisible(false);
			mHighScoresTableLabel->SetVisible(false);
			mHighScoresExitLabel->SetVisible(false);
			// Show main menu labels
			mTitleLabel->SetVisible(true);
			mStartLabel->SetVisible(true);
			mDifficultyLabel->SetVisible(true);
			mInstructionsLabel->SetVisible(true);
			mHighScoresLabel->SetVisible(true);
			// Highlight selected option
			mStartLabel->SetColor(GLVector3f(1.0f, 1.0f, mSelectedOption == 0 ? 0.0f : 1.0f)); // Yellow if selected
			mDifficultyLabel->SetColor(GLVector3f(1.0f, 1.0f, mSelectedOption == 1 ? 0.0f : 1.0f));
			mInstructionsLabel->SetColor(GLVector3f(1.0f, 1.0f, mSelectedOption == 2 ? 0.0f : 1.0f));
			mHighScoresLabel->SetColor(GLVector3f(1.0f, 1.0f, mSelectedOption == 3 ? 0.0f : 1.0f));
		}
	}
	else if (mGameState == STATE_PLAYING) {
		switch (key)
		{
		case ' ':
			mSpaceship->Shoot();
			break;
		default:
			break;
		}
	}
}

void Asteroids::OnKeyReleased(uchar key, int x, int y) {}

void Asteroids::OnSpecialKeyPressed(int key, int x, int y)
{
	if (mGameState == STATE_MAIN_MENU) {
		if (key == GLUT_KEY_UP) {
			mSelectedOption = (mSelectedOption - 1 + 4) % 4; // Cycle up
			// Update colors to highlight selected option
			mStartLabel->SetColor(GLVector3f(1.0f, 1.0f, mSelectedOption == 0 ? 0.0f : 1.0f));
			mDifficultyLabel->SetColor(GLVector3f(1.0f, 1.0f, mSelectedOption == 1 ? 0.0f : 1.0f));
			mInstructionsLabel->SetColor(GLVector3f(1.0f, 1.0f, mSelectedOption == 2 ? 0.0f : 1.0f));
			mHighScoresLabel->SetColor(GLVector3f(1.0f, 1.0f, mSelectedOption == 3 ? 0.0f : 1.0f));
		}
		else if (key == GLUT_KEY_DOWN) {
			mSelectedOption = (mSelectedOption + 1) % 4; // Cycle down
			// Update colors to highlight selected option
			mStartLabel->SetColor(GLVector3f(1.0f, 1.0f, mSelectedOption == 0 ? 0.0f : 1.0f));
			mDifficultyLabel->SetColor(GLVector3f(1.0f, 1.0f, mSelectedOption == 1 ? 0.0f : 1.0f));
			mInstructionsLabel->SetColor(GLVector3f(1.0f, 1.0f, mSelectedOption == 2 ? 0.0f : 1.0f));
			mHighScoresLabel->SetColor(GLVector3f(1.0f, 1.0f, mSelectedOption == 3 ? 0.0f : 1.0f));
		}
	}
	else if (mGameState == STATE_DIFFICULTY_MENU) {
		if (key == GLUT_KEY_UP) {
			mSelectedOption = (mSelectedOption - 1 + 3) % 3; // Cycle up (3 options)
			// Update colors to highlight selected option
			mEasyLabel->SetColor(GLVector3f(1.0f, 1.0f, mSelectedOption == 0 ? 0.0f : 1.0f));
			mNormalLabel->SetColor(GLVector3f(1.0f, 1.0f, mSelectedOption == 1 ? 0.0f : 1.0f));
			mHardLabel->SetColor(GLVector3f(1.0f, 1.0f, mSelectedOption == 2 ? 0.0f : 1.0f));
		}
		else if (key == GLUT_KEY_DOWN) {
			mSelectedOption = (mSelectedOption + 1) % 3; // Cycle down (3 options)
			// Update colors to highlight selected option
			mEasyLabel->SetColor(GLVector3f(1.0f, 1.0f, mSelectedOption == 0 ? 0.0f : 1.0f));
			mNormalLabel->SetColor(GLVector3f(1.0f, 1.0f, mSelectedOption == 1 ? 0.0f : 1.0f));
			mHardLabel->SetColor(GLVector3f(1.0f, 1.0f, mSelectedOption == 2 ? 0.0f : 1.0f));
		}
	}
	else if (mGameState == STATE_PLAYING) {
		switch (key)
		{
			// If up arrow key is pressed start applying forward thrust
		case GLUT_KEY_UP: mSpaceship->Thrust(10); break;
			// If left arrow key is pressed start rotating anti-clockwise
		case GLUT_KEY_LEFT: mSpaceship->Rotate(90); break;
			// If right arrow key is pressed start rotating clockwise
		case GLUT_KEY_RIGHT: mSpaceship->Rotate(-90); break;
			// Default case - do nothing
		default: break;
		}
	}
}

void Asteroids::OnSpecialKeyReleased(int key, int x, int y)
{
	if (mGameState == STATE_PLAYING) {
		switch (key)
		{
			// If up arrow key is released stop applying forward thrust
		case GLUT_KEY_UP: mSpaceship->Thrust(0); break;
			// If left arrow key is released stop rotating
		case GLUT_KEY_LEFT: mSpaceship->Rotate(0); break;
			// If right arrow key is released stop rotating
		case GLUT_KEY_RIGHT: mSpaceship->Rotate(0); break;
			// Default case - do nothing
		default: break;
		}
	}
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IGameWorldListener ////////////////////

void Asteroids::OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
{
	if (mGameState == STATE_PLAYING && object->GetType() == GameObjectType("Asteroid"))
	{
		shared_ptr<GameObject> explosion = CreateExplosion();
		explosion->SetPosition(object->GetPosition());
		explosion->SetRotation(object->GetRotation());
		mGameWorld->AddObject(explosion);
		mAsteroidCount--;
		if (mAsteroidCount <= 0)
		{
			SetTimer(500, START_NEXT_LEVEL);
		}
	}
}

// PUBLIC INSTANCE METHODS IMPLEMENTING ITimerListener ////////////////////////

void Asteroids::OnTimer(int value)
{
	if (mGameState == STATE_PLAYING) {
		if (value == CREATE_NEW_PLAYER)
		{
			mSpaceship->Reset();
			mGameWorld->AddObject(mSpaceship);
		}

		if (value == START_NEXT_LEVEL)
		{
			mLevel++;
			int num_asteroids = 10 + 2 * mLevel;
			CreateAsteroids(num_asteroids);
		}

		if (value == SHOW_GAME_OVER)
		{
			mGameOverLabel->SetVisible(true);
			// Placeholder for gamer tag input (to be implemented later)
			// For now, transition back to main menu
			mGameState = STATE_MAIN_MENU;
			mSelectedOption = 3; // Highlight High Scores
			mLivesLabel->SetVisible(false);
			mScoreLabel->SetVisible(false);
			mGameOverLabel->SetVisible(false);
			mTitleLabel->SetVisible(true);
			mStartLabel->SetVisible(true);
			mDifficultyLabel->SetVisible(true);
			mInstructionsLabel->SetVisible(true);
			mHighScoresLabel->SetVisible(true);
			// Highlight selected option
			mStartLabel->SetColor(GLVector3f(1.0f, 1.0f, 1.0f));
			mDifficultyLabel->SetColor(GLVector3f(1.0f, 1.0f, 1.0f));
			mInstructionsLabel->SetColor(GLVector3f(1.0f, 1.0f, 1.0f));
			mHighScoresLabel->SetColor(GLVector3f(1.0f, 1.0f, 0.0f));
		}
	}
}

// PROTECTED INSTANCE METHODS /////////////////////////////////////////////////

shared_ptr<GameObject> Asteroids::CreateSpaceship()
{
	// Create a raw pointer to a spaceship that can be converted to
	// shared_ptrs of different types because GameWorld implements IRefCount
	mSpaceship = make_shared<Spaceship>();
	mSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mSpaceship->GetThisPtr(), 4.0f));
	shared_ptr<Shape> bullet_shape = make_shared<Shape>("bullet.shape");
	mSpaceship->SetBulletShape(bullet_shape);
	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
	shared_ptr<Sprite> spaceship_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	mSpaceship->SetSprite(spaceship_sprite);
	mSpaceship->SetScale(0.1f);
	// Reset spaceship back to centre of the world
	mSpaceship->Reset();
	// Return the spaceship so it can be added to the world
	return mSpaceship;
}

void Asteroids::CreateAsteroids(const uint num_asteroids)
{
	mAsteroidCount = num_asteroids;
	for (uint i = 0; i < num_asteroids; i++)
	{
		Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("asteroid1");
		shared_ptr<Sprite> asteroid_sprite
			= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
		asteroid_sprite->SetLoopAnimation(true);
		shared_ptr<GameObject> asteroid = make_shared<Asteroid>();
		asteroid->SetBoundingShape(make_shared<BoundingSphere>(asteroid->GetThisPtr(), 10.0f));
		asteroid->SetSprite(asteroid_sprite);
		asteroid->SetScale(0.2f);
		mGameWorld->AddObject(asteroid);
	}
}

void Asteroids::CreateGUI()
{
	// Add a (transparent) border around the edge of the game display
	mGameDisplay->GetContainer()->SetBorder(GLVector2i(10, 10));
	// Create a new GUILabel and wrap it up in a shared_ptr
	mScoreLabel = make_shared<GUILabel>("Score: 0");
	// Set the vertical alignment of the label to GUI_VALIGN_TOP
	mScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> score_component
		= static_pointer_cast<GUIComponent>(mScoreLabel);
	mGameDisplay->GetContainer()->AddComponent(score_component, GLVector2f(0.0f, 1.0f));
	// Initially hide score label
	mScoreLabel->SetVisible(false);

	// Create a new GUILabel and wrap it up in a shared_ptr
	mLivesLabel = make_shared<GUILabel>("Lives: 3");
	// Set the vertical alignment of the label to GUI_VALIGN_BOTTOM
	mLivesLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	// Add the GUILabel to the GUIComponent  
	shared_ptr<GUIComponent> lives_component = static_pointer_cast<GUIComponent>(mLivesLabel);
	mGameDisplay->GetContainer()->AddComponent(lives_component, GLVector2f(0.0f, 0.0f));
	// Initially hide lives label
	mLivesLabel->SetVisible(false);

	// Create a new GUILabel and wrap it up in a shared_ptr
	mGameOverLabel = make_shared<GUILabel>("GAME OVER");
	// Set the horizontal alignment of the label to GUI_HALIGN_CENTER
	mGameOverLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	// Set the vertical alignment of the label to GUI_VALIGN_MIDDLE
	mGameOverLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	// Set the visibility of the label to false (hidden)
	mGameOverLabel->SetVisible(false);
	// Add the GUILabel to the GUIContainer  
	shared_ptr<GUIComponent> game_over_component
		= static_pointer_cast<GUIComponent>(mGameOverLabel);
	mGameDisplay->GetContainer()->AddComponent(game_over_component, GLVector2f(0.5f, 0.5f));

	// Main menu labels
	mTitleLabel = make_shared<GUILabel>("Asteroids");
	mTitleLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mTitleLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mGameDisplay->GetContainer()->AddComponent(mTitleLabel, GLVector2f(0.5f, 0.7f));

	mStartLabel = make_shared<GUILabel>("Start");
	mStartLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mStartLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mStartLabel->SetColor(GLVector3f(1.0f, 1.0f, 0.0f)); // Yellow (selected)
	mGameDisplay->GetContainer()->AddComponent(mStartLabel, GLVector2f(0.5f, 0.5f));

	mDifficultyLabel = make_shared<GUILabel>("Difficulty");
	mDifficultyLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mDifficultyLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mGameDisplay->GetContainer()->AddComponent(mDifficultyLabel, GLVector2f(0.5f, 0.4f));

	mInstructionsLabel = make_shared<GUILabel>("Instructions");
	mInstructionsLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mInstructionsLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mGameDisplay->GetContainer()->AddComponent(mInstructionsLabel, GLVector2f(0.5f, 0.3f));

	mHighScoresLabel = make_shared<GUILabel>("High Scores");
	mHighScoresLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mHighScoresLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mGameDisplay->GetContainer()->AddComponent(mHighScoresLabel, GLVector2f(0.5f, 0.2f));

	// Difficulty submenu labels
	mDifficultyTitleLabel = make_shared<GUILabel>("Select Difficulty");
	mDifficultyTitleLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mDifficultyTitleLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mDifficultyTitleLabel->SetVisible(false);
	mGameDisplay->GetContainer()->AddComponent(mDifficultyTitleLabel, GLVector2f(0.5f, 0.7f));

	mEasyLabel = make_shared<GUILabel>("Easy");
	mEasyLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mEasyLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mEasyLabel->SetVisible(false);
	mGameDisplay->GetContainer()->AddComponent(mEasyLabel, GLVector2f(0.5f, 0.5f));

	mNormalLabel = make_shared<GUILabel>("Normal");
	mNormalLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mNormalLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mNormalLabel->SetVisible(false);
	mGameDisplay->GetContainer()->AddComponent(mNormalLabel, GLVector2f(0.5f, 0.4f));

	mHardLabel = make_shared<GUILabel>("Hard");
	mHardLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mHardLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mHardLabel->SetVisible(false);
	mGameDisplay->GetContainer()->AddComponent(mHardLabel, GLVector2f(0.5f, 0.3f));

	mDifficultyExitLabel = make_shared<GUILabel>("Press spacebar to exit to the main menu");
	mDifficultyExitLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mDifficultyExitLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mDifficultyExitLabel->SetVisible(false);
	mGameDisplay->GetContainer()->AddComponent(mDifficultyExitLabel, GLVector2f(0.5f, 0.1f));

	// Instructions screen label
	mInstructionsTextLabel = make_shared<GUILabel>("Use arrows to change velocity in the desired direction\nPress spacebar to shoot bullets to destroy asteroids\n\nPress spacebar to exit to the main menu");
	mInstructionsTextLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mInstructionsTextLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mInstructionsTextLabel->SetVisible(false);
	mGameDisplay->GetContainer()->AddComponent(mInstructionsTextLabel, GLVector2f(0.5f, 0.5f));

	// High scores screen labels
	mHighScoresTitleLabel = make_shared<GUILabel>("High Scores");
	mHighScoresTitleLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mHighScoresTitleLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mHighScoresTitleLabel->SetVisible(false);
	mGameDisplay->GetContainer()->AddComponent(mHighScoresTitleLabel, GLVector2f(0.5f, 0.7f));

	mHighScoresTableLabel = make_shared<GUILabel>("1. Player1: 1000\n2. Player2: 800\n3. Player3: 600\n4. Player4: 400\n5. Player5: 200");
	mHighScoresTableLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mHighScoresTableLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mHighScoresTableLabel->SetVisible(false);
	mGameDisplay->GetContainer()->AddComponent(mHighScoresTableLabel, GLVector2f(0.5f, 0.5f));

	mHighScoresExitLabel = make_shared<GUILabel>("Press spacebar to exit to the main menu");
	mHighScoresExitLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mHighScoresExitLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mHighScoresExitLabel->SetVisible(false);
	mGameDisplay->GetContainer()->AddComponent(mHighScoresExitLabel, GLVector2f(0.5f, 0.1f));
}

void Asteroids::OnScoreChanged(int score)
{
	// Format the score message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Score: " << score;
	// Get the score message as a string
	std::string score_msg = msg_stream.str();
	mScoreLabel->SetText(score_msg);
}

void Asteroids::OnPlayerKilled(int lives_left)
{
	shared_ptr<GameObject> explosion = CreateExplosion();
	explosion->SetPosition(mSpaceship->GetPosition());
	explosion->SetRotation(mSpaceship->GetRotation());
	mGameWorld->AddObject(explosion);

	// Format the lives left message using an string-based stream
	std::ostringstream msg_stream;
	msg_stream << "Lives: " << lives_left;
	// Get the lives left message as a string
	std::string lives_msg = msg_stream.str();
	mLivesLabel->SetText(lives_msg);

	if (lives_left > 0)
	{
		SetTimer(1000, CREATE_NEW_PLAYER);
	}
	else
	{
		SetTimer(500, SHOW_GAME_OVER);
	}
}

shared_ptr<GameObject> Asteroids::CreateExplosion()
{
	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("explosion");
	shared_ptr<Sprite> explosion_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	explosion_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> explosion = make_shared<Explosion>();
	explosion->SetSprite(explosion_sprite);
	explosion->Reset();
	return explosion;
}

void Asteroids::ResetSpaceship()
{
	mSpaceship->Reset();
}