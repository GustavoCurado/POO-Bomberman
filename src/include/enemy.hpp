#pragma once

#include "entity.hpp"

#define ENEMY_SPEED 2.f

class Enemy : public Entity
{
public:
	Enemy();

	friend bool operator==(const Enemy& enemy1, const Enemy& enemy2);
};
