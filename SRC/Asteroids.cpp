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

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

Asteroids::Asteroids(int argc, char* argv[])
    : GameSession(argc, argv), mIsStartScreen(true), mSelectedMenuOption(0), mShowingInstructions(false)
{
    mLevel = 0;
    mAsteroidCount = 0;
    // Initialize to start screen with menu navigation. High-score and difficulty functionality
    // will be added later
}

/** Destructor. */
Asteroids::~Asteroids(void)
{
}

/** Start an asteroids game. */
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

    // Design note: Only create asteroids for the start screen background. Spaceship is created when
    // the player selects "Start Game" to avoid gameplay during the menu.
    CreateAsteroids(10);
    CreateGUI();

    if (mIsStartScreen)
    {
        mScoreLabel->SetVisible(false);
        mLivesLabel->SetVisible(false);
        mGameOverLabel->SetVisible(false);
        mStartGameLabel->SetVisible(!mShowingInstructions);
        mDifficultyLabel->SetVisible(!mShowingInstructions);
        mInstructionsLabel->SetVisible(!mShowingInstructions);
        mHighScoresLabel->SetVisible(!mShowingInstructions);
        mTitleLabel->SetVisible(!mShowingInstructions);
        mInstructionsThrust->SetVisible(mShowingInstructions);
        mInstructionsDirection->SetVisible(mShowingInstructions);
        mInstructionsShoot->SetVisible(mShowingInstructions);
        mInstructionsGoal->SetVisible(mShowingInstructions);
        mInstructionsExit->SetVisible(mShowingInstructions);
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
    }

    mGameWorld->AddListener(&mPlayer);
    mPlayer.AddListener(thisPtr);

    GameSession::Start();
}

/** Stop the current game. */
void Asteroids::Stop()
{
    GameSession::Stop();
}

// PUBLIC INSTANCE METHODS IMPLEMENTING IKeyboardListener /////////////////////

void Asteroids::OnKeyPressed(uchar key, int x, int y)
{
    if (mIsStartScreen)
    {
        if (mShowingInstructions)
        {
            // Design note: Spacebar returns to main menu from instructions. This is a simple navigation
            // mechanism that will be reused for high scores in a later commit.
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
            // Design note: Spacebar selects the highlighted menu option. Only Start Game and Instructions
            // have functionality at this stage. Difficult Mode and High Scores are placeholders for future commits.
            if (key == ' ')
            {
                switch (mSelectedMenuOption)
                {
                case 0: // Start Game
                    StartGame();
                    break;
                case 1: // Difficult Mode
                    // Placeholder: No functionality yet. Will add toggle for power-ups in a later commit.
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
                    // Placeholder: No high-score display yet. Will add loading/saving and name input in a later commit.
                    break;
                }
            }
        }
    }
    else
    {
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
    if (mIsStartScreen && !mShowingInstructions)
    {
        // Design note: Up/down arrows cycle through menu options. Using a simple integer to track
        // selection, with visual feedback via text prefix. Color-based highlighting or fonts may be added later.
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

// PUBLIC INSTANCE METHODS IMPLEMENTING ITimerListener ////////////////////////

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
    }
    else if (value == SHOW_GAME_OVER)
    {
        mGameOverLabel->SetVisible(true);
        // Design note: After game over, show GAME OVER label. Will add transition back to start screen
        // with name input for high scores in a later commit.
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

void Asteroids::CreateGUI()
{
    mGameDisplay->GetContainer()->SetBorder(GLVector2i(10, 10));

    // Design note: All labels use default font (FONT_9_BY_15) for simplicity. Will add font
    // customization (e.g., Helvetica 18 for title) in a later commit to enhance visual appeal.
    mStartGameLabel = make_shared<GUILabel>("> Start Game");
    mStartGameLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
    mStartGameLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
    mStartGameLabel->SetVisible(mIsStartScreen && !mShowingInstructions);
    shared_ptr<GUIComponent> start_game_component = static_pointer_cast<GUIComponent>(mStartGameLabel);
    mGameDisplay->GetContainer()->AddComponent(start_game_component, GLVector2f(0.5f, 0.65f));

    mDifficultyLabel = make_shared<GUILabel>("Difficult Mode");
    mDifficultyLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
    mDifficultyLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
    mDifficultyLabel->SetVisible(mIsStartScreen && !mShowingInstructions);
    shared_ptr<GUIComponent> difficulty_component = static_pointer_cast<GUIComponent>(mDifficultyLabel);
    mGameDisplay->GetContainer()->AddComponent(difficulty_component, GLVector2f(0.5f, 0.55f));

    mInstructionsLabel = make_shared<GUILabel>("Instructions");
    mInstructionsLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
    mInstructionsLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
    mInstructionsLabel->SetVisible(mIsStartScreen && !mShowingInstructions);
    shared_ptr<GUIComponent> instructions_component = static_pointer_cast<GUIComponent>(mInstructionsLabel);
    mGameDisplay->GetContainer()->AddComponent(instructions_component, GLVector2f(0.5f, 0.45f));

    mHighScoresLabel = make_shared<GUILabel>("High Scores");
    mHighScoresLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
    mHighScoresLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
    mHighScoresLabel->SetVisible(mIsStartScreen && !mShowingInstructions);
    shared_ptr<GUIComponent> high_scores_component = static_pointer_cast<GUIComponent>(mHighScoresLabel);
    mGameDisplay->GetContainer()->AddComponent(high_scores_component, GLVector2f(0.5f, 0.35f));

    mTitleLabel = make_shared<GUILabel>("ASTEROIDS");
    mTitleLabel->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
    mTitleLabel->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
    mTitleLabel->SetVisible(mIsStartScreen && !mShowingInstructions);
    shared_ptr<GUIComponent> title_component = static_pointer_cast<GUIComponent>(mTitleLabel);
    mGameDisplay->GetContainer()->AddComponent(title_component, GLVector2f(0.5f, 0.85f));

    // Design note: Instructions are split into multiple labels for clarity and alignment.
    // Will consider consolidating into a single multiline label or adjusting fonts in a later commit.
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
    mInstructionsExit->SetHorizontalAlignment(GUIComponent::GUI_HALIGN_CENTER);
    mInstructionsExit->SetVerticalAlignment(GUIComponent::GUI_VALIGN_MIDDLE);
    mInstructionsExit->SetVisible(mShowingInstructions);
    shared_ptr<GUIComponent> instructions_exit_component = static_pointer_cast<GUIComponent>(mInstructionsExit);
    mGameDisplay->GetContainer()->AddComponent(instructions_exit_component, GLVector2f(0.5f, 0.35f));

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

void Asteroids::OnScoreChanged(int score)
{
    std::ostringstream msg_stream;
    msg_stream << "Score: " << score;
    std::string score_msg = msg_stream.str();
    mScoreLabel->SetText(score_msg);
}

void Asteroids::OnPlayerKilled(int lives_left)
{
    shared_ptr<GameObject> explosion = CreateExplosion();
    explosion->SetPosition(mSpaceship->GetPosition());
    explosion->SetRotation(mSpaceship->GetRotation());
    mGameWorld->AddObject(explosion);

    std::ostringstream msg_stream;
    msg_stream << "Lives: " << lives_left;
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

void Asteroids::UpdateMenuDisplay()
{
    // Design note: Use ">" prefix to indicate selection. Will consider adding color-based highlighting
    // or font changes in a later commit to improve visual feedback.
    mStartGameLabel->SetText(mSelectedMenuOption == 0 ? "> Start Game" : "Start Game");
    mDifficultyLabel->SetText(mSelectedMenuOption == 1 ? "> Difficult Mode" : "Difficult Mode");
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
    mScoreLabel->SetVisible(true);
    mLivesLabel->SetVisible(true);
    mGameWorld->AddObject(CreateSpaceship());
    // Design note: Transition to gameplay by spawning spaceship and showing lives/score.
    // Future commits will add difficulty settings and high-score integration.
}