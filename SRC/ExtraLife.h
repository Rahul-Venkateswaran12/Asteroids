#ifndef __EXTRALIFE_H__
#define __EXTRALIFE_H__

#include "GameObject.h"
#include "Player.h"

class ExtraLife : public GameObject
{
public:
	ExtraLife(Player* player);
	~ExtraLife(void);

	bool CollisionTest(shared_ptr<GameObject> o);
	void OnCollision(const GameObjectList& objects);

private:
	Player* mPlayer; // Pointer to Player for life increment
};

#endif