#include "ExtraLife.h"
#include "GameUtil.h"
#include "BoundingShape.h"
#include "BoundingSphere.h"
#include <cstdlib>

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

ExtraLife::ExtraLife(Player* player) : GameObject("ExtraLife"), mPlayer(player)
{
	// Design note: Randomize position and velocity like Asteroid for consistent spawning.
	mAngle = rand() % 360;
	mRotation = 0;
	mPosition.x = (rand() % 200 - 100); // World width is 200
	mPosition.y = (rand() % 200 - 100); // World height is 200
	mPosition.z = 0.0;
	mVelocity.x = 5.0 * cos(DEG2RAD * mAngle); // Slower than asteroids (10.0)
	mVelocity.y = 5.0 * sin(DEG2RAD * mAngle);
	mVelocity.z = 0.0;
}

ExtraLife::~ExtraLife(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

bool ExtraLife::CollisionTest(shared_ptr<GameObject> o)
{
	// Design note: Only collide with Spaceship, ignore all other objects (e.g., Asteroid, Bullet).
	if (o->GetType() == GameObjectType("Spaceship"))
	{
		if (mBoundingShape.get() && o->GetBoundingShape().get())
		{
			return mBoundingShape->CollisionTest(o->GetBoundingShape());
		}
	}
	return false;
}

void ExtraLife::OnCollision(const GameObjectList& objects)
{
	// Design note: On collision with Spaceship, increment lives and remove heart.
	for (const auto& obj : objects)
	{
		if (obj->GetType() == GameObjectType("Spaceship"))
		{
			if (mPlayer)
			{
				mPlayer->AddLife();
				mWorld->FlagForRemoval(GetThisPtr());
			}
			break;
		}
	}
}