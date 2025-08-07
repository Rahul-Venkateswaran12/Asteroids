#include "ExtraLife.h"
#include "GameUtil.h"
#include "BoundingShape.h" // Added for BoundingShape definition
#include "BoundingSphere.h" // Added for consistency with Asteroids::CreateExtraLife

// PUBLIC INSTANCE CONSTRUCTORS ///////////////////////////////////////////////

/** Default constructor. */
ExtraLife::ExtraLife(void) : GameObject("ExtraLife")
{
}

/** Destructor. */
ExtraLife::~ExtraLife(void)
{
}

// PUBLIC INSTANCE METHODS ////////////////////////////////////////////////////

/** Test for collision with another game object. */
bool ExtraLife::CollisionTest(shared_ptr<GameObject> o)
{
	// Design note: Use a bounding sphere for collision detection, similar to Asteroid.
	// Will implement spaceship collision to grant extra life in a later commit.
	if (o->GetType() == GameObjectType("Spaceship"))
	{
		if (mBoundingShape.get() && o->GetBoundingShape().get())
		{
			return mBoundingShape->CollisionTest(o->GetBoundingShape());
		}
	}
	return false;
}

/** Handle collision with other objects. */
void ExtraLife::OnCollision(const GameObjectList& objects)
{
	// Design note: Mark the extra life for removal upon collision with the spaceship.
	// Actual life increment will be implemented in a later commit.
	mWorld->FlagForRemoval(GetThisPtr());
}