#include "GameUtil.h"
#include "GameWorld.h"
#include "Bullet.h"
#include "Spaceship.h"
#include "BoundingSphere.h"
#include "Explosion.h" // Added for Explosion class
#include "Animation.h" // Added for Animation class
#include "AnimationManager.h" // Added for AnimationManager class

using namespace std;

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

Spaceship::Spaceship()
	: GameObject("Spaceship"), mThrust(0)
{
}

Spaceship::Spaceship(GLVector3f p, GLVector3f v, GLVector3f a, GLfloat h, GLfloat r)
	: GameObject("Spaceship", p, v, a, h, r), mThrust(0)
{
}

Spaceship::Spaceship(const Spaceship& s)
	: GameObject(s), mThrust(0)
{
}

Spaceship::~Spaceship(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

void Spaceship::Update(int t)
{
	GameObject::Update(t);
}

void Spaceship::Render(void)
{
	if (mSpaceshipShape.get() != NULL) mSpaceshipShape->Render();
	if ((mThrust > 0) && (mThrusterShape.get() != NULL)) {
		mThrusterShape->Render();
	}
	GameObject::Render();
}

void Spaceship::Thrust(float t)
{
	mThrust = t;
	mAcceleration.x = mThrust * cos(DEG2RAD * mAngle);
	mAcceleration.y = mThrust * sin(DEG2RAD * mAngle);
}

void Spaceship::Rotate(float r)
{
	mRotation = r;
}

void Spaceship::Shoot(void)
{
	if (!mWorld) return;
	GLVector3f spaceship_heading(cos(DEG2RAD * mAngle), sin(DEG2RAD * mAngle), 0);
	spaceship_heading.normalize();
	GLVector3f bullet_position = mPosition + (spaceship_heading * 4);
	float bullet_speed = 30;
	GLVector3f bullet_velocity = mVelocity + spaceship_heading * bullet_speed;
	shared_ptr<GameObject> bullet
	(new Bullet(bullet_position, bullet_velocity, mAcceleration, mAngle, 0, 2000));
	bullet->SetBoundingShape(make_shared<BoundingSphere>(bullet->GetThisPtr(), 2.0f));
	bullet->SetShape(mBulletShape);
	mWorld->AddObject(bullet);
}

bool Spaceship::CollisionTest(shared_ptr<GameObject> o)
{
	// Design note: Only collide with Asteroids, ignore ExtraLife.
	if (o->GetType() != GameObjectType("Asteroid"))
		return false;
	if (mBoundingShape.get() == NULL) return false;
	if (o->GetBoundingShape().get() == NULL) return false;
	return mBoundingShape->CollisionTest(o->GetBoundingShape());
}

void Spaceship::OnCollision(const GameObjectList& objects)
{
	// Design note: Only flag for removal on asteroid collision, spawn explosion here.
	for (const auto& obj : objects)
	{
		if (obj->GetType() == GameObjectType("Asteroid"))
		{
			shared_ptr<GameObject> explosion = make_shared<Explosion>();
			Animation* anim_ptr = AnimationManager::GetInstance().GetAnimationByName("explosion");
			shared_ptr<Sprite> explosion_sprite =
				make_shared<Sprite>(anim_ptr->GetWidth(), anim_ptr->GetHeight(), anim_ptr);
			explosion_sprite->SetLoopAnimation(false);
			explosion->SetSprite(explosion_sprite); // Fixed typo: extrusion_sprite to explosion_sprite
			explosion->SetPosition(GetPosition());
			explosion->SetRotation(GetRotation());
			explosion->Reset();
			mWorld->AddObject(explosion);
			mWorld->FlagForRemoval(GetThisPtr());
			break;
		}
	}
}