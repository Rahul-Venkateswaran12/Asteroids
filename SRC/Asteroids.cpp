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
#include "GUIContainer.h"
#include "ExtraLife.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

Asteroids::Asteroids(int argc, char* argv[])
	: GameSession(argc, argv), mIsStartScreen(true), mSelectedMenuOption(0), mEnablePowerups(false),
	mShowingInstructions(false), mShowingHighScores(false), mEnteringName(false), mCurrentName(""),
	mLastSpawnAttemptTime(0)
{
	mLevel = 0;
	mAsteroidCount = 0;
	LoadHighScores();
}

Asteroids::~Asteroids(void)
{
}

void Asteroids::Start()
{
	shared_ptr<Asteroids> thisPtr = shared_ptr<Asteroids>(this);
	mGameWorld->AddListener(thisPtr.get());
	mGameWindow->AddKeyboardListener(thisPtr);
	mGameWorld->AddListener(&mScoreKeeper);
	mScoreKeeper.AddListener(thisPtr);

	GLfloat ambient_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat diffuse_light[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient_light);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse_light);
	glEnable(GL_LIGHT0);

	Animation* explosion_anim = AnimationManager::GetInstance().CreateAnimationFromFile("explosion", 64, 1024, 64, 64, "explosion_fs.png");
	Animation* asteroid1_anim = AnimationManager::GetInstance().CreateAnimationFromFile("asteroid1", 128, 8192, 128, 128, "asteroid1_fs.png");
	Animation* spaceship_anim = AnimationManager::GetInstance().CreateAnimationFromFile("spaceship", 128, 128, 128, 128, "spaceship_fs.png");
	Animation* extralife_anim = AnimationManager::GetInstance().CreateAnimationFromFile("extralife", 32, 32, 32, 32, "extralife_fs.png");

	if (!mIsStartScreen)
	{
		mGameWorld->AddObject(CreateSpaceship());
	}

	CreateAsteroids(10);
	// Removed CreateExtraLife() to avoid initial spawn
	CreateGUI();

	if (mIsStartScreen)
	{
		mScoreLabel->SetVisible(false);
		mLivesLabel->SetVisible(false);
		mGameOverLabel->SetVisible(false);
		mStartGameLabel->SetVisible(!mShowingInstructions && !mShowingHighScores && !mEnteringName);
		mDifficultyLabel->SetVisible(!mShowingInstructions && !mShowingHighScores && !mEnteringName);
		mInstructionsLabel->SetVisible(!mShowingInstructions && !mShowingHighScores && !mEnteringName);
		mHighScoresLabel->SetVisible(!mShowingInstructions && !mShowingHighScores && !mEnteringName);
		mTitleLabel->SetVisible(!mShowingInstructions && !mShowingHighScores && !mEnteringName);
		mInstructionsThrust->SetVisible(mShowingInstructions);
		mInstructionsDirection->SetVisible(mShowingInstructions);
		mInstructionsShoot->SetVisible(mShowingInstructions);
		mInstructionsGoal->SetVisible(mShowingInstructions);
		mInstructionsExit->SetVisible(mShowingInstructions);
		for (int i = 0; i < 5; ++i)
			mHighScoreLabels[i]->SetVisible(mShowingHighScores);
		mHighScoreExitLabel->SetVisible(mShowingHighScores);
		mNameInputLabel->SetVisible(mEnteringName);
		UpdateMenuDisplay();
	}
	else
	{
		mStartGameLabel->SetVisible(false);
		mDifficultyLabel->SetVisible(false);
		mInstructionsLabel->SetVisible(false);
		mHighScoresLabel->SetVisible(false);
		mTitleLabel->SetVisible(false);
		mInstructionsThrust->SetVisible(false);
		mInstructionsDirection->SetVisible(false);
		mInstructionsShoot->SetVisible(false);
		mInstructionsGoal->SetVisible(false);
		mInstructionsExit->SetVisible(false);
		for (int i = 0; i < 5; ++i)
			mHighScoreLabels[i]->SetVisible(false);
		mHighScoreExitLabel->SetVisible(false);
		mNameInputLabel->SetVisible(false);
	}

	mGameWorld->AddListener(&mPlayer);
	mPlayer.AddListener(thisPtr);

	GameSession::Start();
}

void Asteroids::Stop()
{
	GameSession::Stop();
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IKeyboardListener /////////////////////

void Asteroids::OnKeyPressed(uchar key, int x, int y)
{
	if (mIsStartScreen)
	{
		if (mEnteringName)
		{
			if (key == 13) // Enter key
			{
				if (!mCurrentName.empty())
				{
					AddHighScore(mCurrentName, mScoreKeeper.GetScore());
					SaveHighScores();
					mCurrentName.clear();
					mEnteringName = false;
					mNameInputLabel->SetVisible(false);
					mStartGameLabel->SetVisible(true);
					mDifficultyLabel->SetVisible(true);
					mInstructionsLabel->SetVisible(true);
					mHighScoresLabel->SetVisible(true);
					mTitleLabel->SetVisible(true);
					UpdateMenuDisplay();
				}
			}
			else if (key == 8 && !mCurrentName.empty()) // Backspace
			{
				mCurrentName.pop_back();
				mNameInputLabel->SetText("Enter Name: " + mCurrentName);
			}
			else if (mCurrentName.length() < 8 && ((key >= 'A' && key <= 'Z') || (key >= '0' && key <= '9')))
			{
				mCurrentName += key;
				mNameInputLabel->SetText("Enter Name: " + mCurrentName);
			}
		}
		else if (mShowingHighScores)
		{
			if (key == ' ')
			{
				mShowingHighScores = false;
				mStartGameLabel->SetVisible(true);
				mDifficultyLabel->SetVisible(true);
				mInstructionsLabel->SetVisible(true);
				mHighScoresLabel->SetVisible(true);
				mTitleLabel->SetVisible(true);
				for (int i = 0; i < 5; ++i)
					mHighScoreLabels[i]->SetVisible(false);
				mHighScoreExitLabel->SetVisible(false);
				UpdateMenuDisplay();
			}
		}
		else if (mShowingInstructions)
		{
			if (key == ' ')
			{
				mShowingInstructions = false;
				mStartGameLabel->SetVisible(true);
				mDifficultyLabel->SetVisible(true);
				mInstructionsLabel->SetVisible(true);
				mHighScoresLabel->SetVisible(true);
				mTitleLabel->SetVisible(true);
				mInstructionsThrust->SetVisible(false);
				mInstructionsDirection->SetVisible(false);
				mInstructionsShoot->SetVisible(false);
				mInstructionsGoal->SetVisible(false);
				mInstructionsExit->SetVisible(false);
				UpdateMenuDisplay();
			}
		}
		else
		{
			if (key == ' ')
			{
				switch (mSelectedMenuOption)
				{
				case 0: // Start Game
					StartGame();
					break;
				case 1: // Difficult Mode
					mEnablePowerups = !mEnablePowerups;
					UpdateMenuDisplay();
					break;
				case 2: // Instructions
					mShowingInstructions = true;
					mStartGameLabel->SetVisible(false);
					mDifficultyLabel->SetVisible(false);
					mInstructionsLabel->SetVisible(false);
					mHighScoresLabel->SetVisible(false);
					mTitleLabel->SetVisible(false);
					mInstructionsThrust->SetVisible(true);
					mInstructionsDirection->SetVisible(true);
					mInstructionsShoot->SetVisible(true);
					mInstructionsGoal->SetVisible(true);
					mInstructionsExit->SetVisible(true);
					break;
				case 3: // High Scores
					mShowingHighScores = true;
					mStartGameLabel->SetVisible(false);
					mDifficultyLabel->SetVisible(false);
					mInstructionsLabel->SetVisible(false);
					mHighScoresLabel->SetVisible(false);
					mTitleLabel->SetVisible(false);
					UpdateHighScoreDisplay();
					for (int i = 0; i < 5; ++i)
						mHighScoreLabels[i]->SetVisible(true);
					mHighScoreExitLabel->SetVisible(true);
					break;
				}
			}
		}
	}
	else
	{
		switch (key)
		{
		case ' ': mSpaceship->Shoot(); break;
		default: break;
		}
	}
}

void Asteroids::OnKeyReleased(uchar key, int x, int y) {}

void Asteroids::OnSpecialKeyPressed(int key, int x, int y)
{
	if (mIsStartScreen && !mShowingInstructions && !mShowingHighScores && !mEnteringName)
	{
		if (key == GLUT_KEY_UP)
		{
			mSelectedMenuOption = (mSelectedMenuOption - 1 + 4) % 4;
			UpdateMenuDisplay();
		}
		else if (key == GLUT_KEY_DOWN)
		{
			mSelectedMenuOption = (mSelectedMenuOption + 1) % 4;
			UpdateMenuDisplay();
		}
	}
	else if (!mIsStartScreen)
	{
		switch (key)
		{
		case GLUT_KEY_UP: mSpaceship->Thrust(10); break;
		case GLUT_KEY_LEFT: mSpaceship->Rotate(90); break;
		case GLUT_KEY_RIGHT: mSpaceship->Rotate(-90); break;
		default: break;
		}
	}
}

void Asteroids::OnSpecialKeyReleased(int key, int x, int y)
{
	if (!mIsStartScreen)
	{
		switch (key)
		{
		case GLUT_KEY_UP: mSpaceship->Thrust(0); break;
		case GLUT_KEY_LEFT: mSpaceship->Rotate(0); break;
		case GLUT_KEY_RIGHT: mSpaceship->Rotate(0); break;
		default: break;
		}
	}
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IGameWorldListener ////////////////////

void Asteroids::OnObjectRemoved(GameWorld* world, shared_ptr<GameObject> object)
{
	if (object->GetType() == GameObjectType("Asteroid"))
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

// PUBLIC INSTANCE METHODS IMPLEMENTING IPlayerListener ///////////////////////

void Asteroids::OnPlayerKilled(int lives_left)
{
	// Design note: Handle life decrements (asteroid collisions).
	// Explosion handled in Spaceship::OnCollision.
	std::ostringstream msg_stream;
	msg_stream << "Lives: " << lives_left;
	mLivesLabel->SetText(msg_stream.str());
	if (lives_left <= 0) // Game over
	{
		SetTimer(500, SHOW_GAME_OVER);
	}
	else // Asteroid collision
	{
		SetTimer(1000, CREATE_NEW_PLAYER);
	}
}

void Asteroids::OnLifeGained(int lives_left)
{
	// Design note: Handle life increments (heart collection), update display only.
	std::ostringstream msg_stream;
	msg_stream << "Lives: " << lives_left;
	mLivesLabel->SetText(msg_stream.str());
}

void Asteroids::OnTimer(int value)
{
	if (value == CREATE_NEW_PLAYER)
	{
		mSpaceship->Reset();
		mGameWorld->AddObject(mSpaceship);
	}
	else if (value == START_NEXT_LEVEL)
	{
		mLevel++;
		int num_asteroids = 10 + 2 * mLevel;
		CreateAsteroids(num_asteroids);
		// Removed CreateExtraLife() to avoid level-based spawning
	}
	else if (value == SHOW_GAME_OVER)
	{
		// Design note: Show game over briefly, then show high score input screen
		mGameOverLabel->SetVisible(true);
		SetTimer(2000, 4); // New timer to transition to high score input after 2 seconds
	}
	else if (value == 4) // New timer value for high score input
	{
		mIsStartScreen = true;
		mEnteringName = true;
		mCurrentName = "";
		mGameOverLabel->SetVisible(false);
		mScoreLabel->SetVisible(false);
		mLivesLabel->SetVisible(false);
		mStartGameLabel->SetVisible(false);
		mDifficultyLabel->SetVisible(false);
		mInstructionsLabel->SetVisible(false);
		mHighScoresLabel->SetVisible(false);
		mTitleLabel->SetVisible(false);
		mInstructionsThrust->SetVisible(false);
		mInstructionsDirection->SetVisible(false);
		mInstructionsShoot->SetVisible(false);
		mInstructionsGoal->SetVisible(false);
		mInstructionsExit->SetVisible(false);
		for (int i = 0; i < 5; ++i)
			mHighScoreLabels[i]->SetVisible(false);
		mHighScoreExitLabel->SetVisible(false);
		mNameInputLabel->SetVisible(true);
		mNameInputLabel->SetText("Enter Name: ");
	}
	else if (value == SPAWN_EXTRA_LIFE)
	{
		// Design note: 100% chance to spawn ExtraLife every 2-5s for testing if difficulty is off and no heart exists
		if (!mEnablePowerups && !HasExtraLife())
		{
			if (true) // 100% chance for testing
			{
				CreateExtraLife();
			}
		}
		// Schedule next spawn attempt in 2-5s
		int next_interval = 2000 + (rand() % 3001); // 2,000 to 5,000 ms
		SetTimer(next_interval, SPAWN_EXTRA_LIFE);
		mLastSpawnAttemptTime = glutGet(GLUT_ELAPSED_TIME);
	}
}

// PROTECTED INSTANCE METHODS /////////////////////////////////////////////////

shared_ptr<GameObject> Asteroids::CreateSpaceship()
{
	mSpaceship = make_shared<Spaceship>();
	mSpaceship->SetBoundingShape(make_shared<BoundingSphere>(mSpaceship->GetThisPtr(), 4.0f));
	shared_ptr<Shape> bullet_shape = make_shared<Shape>("bullet.shape");
	mSpaceship->SetBulletShape(bullet_shape);
	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("spaceship");
	shared_ptr<Sprite> spaceship_sprite =
		make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	mSpaceship->SetSprite(spaceship_sprite);
	mSpaceship->SetScale(0.1f);
	mSpaceship->Reset();
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

void Asteroids::CreateExtraLife()
{
	// Design note: Spawns one extra life power-up with random positioning and velocity.
	// Sprite rotated 180 degrees to fix upside-down rendering.
	Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("extralife");
	shared_ptr<Sprite> extralife_sprite
		= make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
	extralife_sprite->SetLoopAnimation(false);
	shared_ptr<GameObject> extralife = make_shared<ExtraLife>(&mPlayer);
	extralife->SetBoundingShape(make_shared<BoundingSphere>(extralife->GetThisPtr(), 4.0f));
	extralife->SetSprite(extralife_sprite);
	extralife->SetScale(0.2f);
	extralife->SetRotation(180.0f);
	mGameWorld->AddObject(extralife);
}

bool Asteroids::HasExtraLife()
{
	// Design note: Check if an ExtraLife object exists in the game world
	for (const auto& obj : mGameWorld->GetObjects())
	{
		if (obj->GetType() == GameObjectType("ExtraLife"))
		{
			return true;
		}
	}
	return false;
}

void Asteroids::CreateGUI()
{
	mGameDisplay->GetContainer()->SetBorder(GLVector2i(10, 10));

	mStartGameLabel = make_shared<GUILabel>("> Start Game");
	mStartGameLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mStartGameLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mStartGameLabel->SetVisible(mIsStartScreen && !mShowingInstructions && !mShowingHighScores && !mEnteringName);
	shared_ptr<GUIComponent> start_game_component = static_pointer_cast<GUIComponent>(mStartGameLabel);
	mGameDisplay->GetContainer()->AddComponent(start_game_component, GLVector2f(0.5f, 0.65f));

	mDifficultyLabel = make_shared<GUILabel>("Difficult Mode: Off");
	mDifficultyLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mDifficultyLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mDifficultyLabel->SetVisible(mIsStartScreen && !mShowingInstructions && !mShowingHighScores && !mEnteringName);
	shared_ptr<GUIComponent> difficulty_component = static_pointer_cast<GUIComponent>(mDifficultyLabel);
	mGameDisplay->GetContainer()->AddComponent(difficulty_component, GLVector2f(0.5f, 0.55f));

	mInstructionsLabel = make_shared<GUILabel>("Instructions");
	mInstructionsLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mInstructionsLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mInstructionsLabel->SetVisible(mIsStartScreen && !mShowingInstructions && !mShowingHighScores && !mEnteringName);
	shared_ptr<GUIComponent> instructions_component = static_pointer_cast<GUIComponent>(mInstructionsLabel);
	mGameDisplay->GetContainer()->AddComponent(instructions_component, GLVector2f(0.5f, 0.45f));

	mHighScoresLabel = make_shared<GUILabel>("High Scores");
	mHighScoresLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mHighScoresLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mHighScoresLabel->SetVisible(mIsStartScreen && !mShowingInstructions && !mShowingHighScores && !mEnteringName);
	shared_ptr<GUIComponent> high_scores_component = static_pointer_cast<GUIComponent>(mHighScoresLabel);
	mGameDisplay->GetContainer()->AddComponent(high_scores_component, GLVector2f(0.5f, 0.35f));

	mTitleLabel = make_shared<GUILabel>("ASTEROIDS", GUILabel::FONT_HELVETICA_18);
	mTitleLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mTitleLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mTitleLabel->SetVisible(mIsStartScreen && !mShowingInstructions && !mShowingHighScores && !mEnteringName);
	shared_ptr<GUIComponent> title_component = static_pointer_cast<GUIComponent>(mTitleLabel);
	mGameDisplay->GetContainer()->AddComponent(title_component, GLVector2f(0.5f, 0.85f));

	mInstructionsThrust = make_shared<GUILabel>("Controls: UP to thrust");
	mInstructionsThrust->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mInstructionsThrust->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mInstructionsThrust->SetVisible(mShowingInstructions);
	shared_ptr<GUIComponent> instructions_thrust_component = static_pointer_cast<GUIComponent>(mInstructionsThrust);
	mGameDisplay->GetContainer()->AddComponent(instructions_thrust_component, GLVector2f(0.5f, 0.75f));

	mInstructionsDirection = make_shared<GUILabel>("LEFT / RIGHT to rotate");
	mInstructionsDirection->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mInstructionsDirection->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mInstructionsDirection->SetVisible(mShowingInstructions);
	shared_ptr<GUIComponent> instructions_direction_component = static_pointer_cast<GUIComponent>(mInstructionsDirection);
	mGameDisplay->GetContainer()->AddComponent(instructions_direction_component, GLVector2f(0.5f, 0.65f));

	mInstructionsShoot = make_shared<GUILabel>("SPACE to shoot");
	mInstructionsShoot->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mInstructionsShoot->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mInstructionsShoot->SetVisible(mShowingInstructions);
	shared_ptr<GUIComponent> instructions_shoot_component = static_pointer_cast<GUIComponent>(mInstructionsShoot);
	mGameDisplay->GetContainer()->AddComponent(instructions_shoot_component, GLVector2f(0.5f, 0.55f));

	mInstructionsGoal = make_shared<GUILabel>("Goal: Destroy asteroids, avoid collisions.");
	mInstructionsGoal->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mInstructionsGoal->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mInstructionsGoal->SetVisible(mShowingInstructions);
	shared_ptr<GUIComponent> instructions_goal_component = static_pointer_cast<GUIComponent>(mInstructionsGoal);
	mGameDisplay->GetContainer()->AddComponent(instructions_goal_component, GLVector2f(0.5f, 0.45f));

	mInstructionsExit = make_shared<GUILabel>("Press SPACE to return.");
	mInstructionsExit->SetVisible(mShowingInstructions);
	shared_ptr<GUIComponent> instructions_exit_component = static_pointer_cast<GUIComponent>(mInstructionsExit);
	mGameDisplay->GetContainer()->AddComponent(instructions_exit_component, GLVector2f(0.5f, 0.35f));

	for (int i = 0; i < 5; ++i)
	{
		mHighScoreLabels[i] = make_shared<GUILabel>("");
		mHighScoreLabels[i]->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
		mHighScoreLabels[i]->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
		mHighScoreLabels[i]->SetVisible(mShowingHighScores);
		shared_ptr<GUIComponent> high_score_component = static_pointer_cast<GUIComponent>(mHighScoreLabels[i]);
		mGameDisplay->GetContainer()->AddComponent(high_score_component, GLVector2f(0.5f, 0.85f - i * 0.1f));
	}

	mHighScoreExitLabel = make_shared<GUILabel>("Press SPACE to return.");
	mHighScoreExitLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mHighScoreExitLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mHighScoreExitLabel->SetVisible(mShowingHighScores);
	shared_ptr<GUIComponent> high_score_exit_component = static_pointer_cast<GUIComponent>(mHighScoreExitLabel);
	mGameDisplay->GetContainer()->AddComponent(high_score_exit_component, GLVector2f(0.5f, 0.35f));

	mNameInputLabel = make_shared<GUILabel>("Enter Name: ");
	mNameInputLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mNameInputLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mNameInputLabel->SetVisible(mEnteringName);
	shared_ptr<GUIComponent> name_input_component = static_pointer_cast<GUIComponent>(mNameInputLabel);
	mGameDisplay->GetContainer()->AddComponent(name_input_component, GLVector2f(0.5f, 0.5f));

	mScoreLabel = make_shared<GUILabel>("Score: 0");
	mScoreLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_TOP);
	shared_ptr<GUIComponent> score_component = static_pointer_cast<GUIComponent>(mScoreLabel);
	mGameDisplay->GetContainer()->AddComponent(score_component, GLVector2f(0.0f, 1.0f));

	mLivesLabel = make_shared<GUILabel>("Lives: 3");
	mLivesLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_BOTTOM);
	shared_ptr<GUIComponent> lives_component = static_pointer_cast<GUIComponent>(mLivesLabel);
	mGameDisplay->GetContainer()->AddComponent(lives_component, GLVector2f(0.0f, 0.0f));

	mGameOverLabel = make_shared<GUILabel>("GAME OVER");
	mGameOverLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
	mGameOverLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
	mGameOverLabel->SetVisible(false);
	shared_ptr<GUIComponent> game_over_component = static_pointer_cast<GUIComponent>(mGameOverLabel);
	mGameDisplay->GetContainer()->AddComponent(game_over_component, GLVector2f(0.5f, 0.5f));
}

void Asteroids::LoadHighScores()
{
	mHighScores.clear();
	std::ifstream file("highscores.txt");
	if (file.is_open())
	{
		std::string name;
		int score;
		while (file >> name >> score)
		{
			mHighScores.emplace_back(name, score);
		}
		file.close();
	}
	while (mHighScores.size() < 5)
	{
		mHighScores.emplace_back("---", 0);
	}
	std::sort(mHighScores.begin(), mHighScores.end(),
		[](const auto& a, const auto& b) { return a.second > b.second; });
}

void Asteroids::SaveHighScores()
{
	std::ofstream file("highscores.txt");
	if (file.is_open())
	{
		for (const auto& entry : mHighScores)
		{
			file << entry.first << " " << entry.second << "\n";
		}
		file.close();
	}
}

void Asteroids::UpdateHighScoreDisplay()
{
	for (int i = 0; i < 5; ++i)
	{
		std::ostringstream oss;
		oss << (i + 1) << ". " << mHighScores[i].first << " " << mHighScores[i].second;
		mHighScoreLabels[i]->SetText(oss.str());
	}
}

void Asteroids::AddHighScore(const std::string& name, int score)
{
	mHighScores.emplace_back(name, score);
	std::sort(mHighScores.begin(), mHighScores.end(),
		[](const auto& a, const auto& b) { return a.second > b.second; });
	if (mHighScores.size() > 5)
		mHighScores.resize(5);
}

void Asteroids::OnScoreChanged(int score)
{
	std::ostringstream msg_stream;
	msg_stream << "Score: " << score;
	std::string score_msg = msg_stream.str();
	mScoreLabel->SetText(score_msg);
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

void Asteroids::UpdateMenuDisplay()
{
	mStartGameLabel->SetText(mSelectedMenuOption == 0 ? "> Start Game" : "Start Game");
	mDifficultyLabel->SetText(mSelectedMenuOption == 1 ? (mEnablePowerups ? "> Difficult Mode: On" : "> Difficult Mode: Off") : (mEnablePowerups ? "Difficult Mode: On" : "Difficult Mode: Off"));
	mInstructionsLabel->SetText(mSelectedMenuOption == 2 ? "> Instructions" : "Instructions");
	mHighScoresLabel->SetText(mSelectedMenuOption == 3 ? "> High Scores" : "High Scores");
}

void Asteroids::StartGame()
{
	mIsStartScreen = false;
	mStartGameLabel->SetVisible(false);
	mDifficultyLabel->SetVisible(false);
	mInstructionsLabel->SetVisible(false);
	mHighScoresLabel->SetVisible(false);
	mTitleLabel->SetVisible(false);
	mInstructionsThrust->SetVisible(false);
	mInstructionsDirection->SetVisible(false);
	mInstructionsShoot->SetVisible(false);
	mInstructionsGoal->SetVisible(false);
	mInstructionsExit->SetVisible(false);
	for (int i = 0; i < 5; ++i)
		mHighScoreLabels[i]->SetVisible(false);
	mHighScoreExitLabel->SetVisible(false);
	mNameInputLabel->SetVisible(false);
	mScoreLabel->SetVisible(true);
	mLivesLabel->SetVisible(true);
	mGameWorld->AddObject(CreateSpaceship());
	// Start ExtraLife spawn timer if difficulty is off
	if (!mEnablePowerups)
	{
		int initial_interval = 2000 + (rand() % 3001); // 2-5s for testing
		SetTimer(initial_interval, SPAWN_EXTRA_LIFE);
		mLastSpawnAttemptTime = glutGet(GLUT_ELAPSED_TIME);
	}
}