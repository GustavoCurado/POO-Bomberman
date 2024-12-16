#pragma once

#include "entity.hpp"

#define BOMBERMAN_SPEED 16.f

class Bomberman : public Entity
{
private:
	bool _is_dead;
public:
	Bomberman();

	bool& is_dead();
};
