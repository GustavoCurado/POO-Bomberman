#pragma once

#define EXPLOSION_RADIUS 3

class Bomb
{
private:
	int _radius;
	bool _is_exploding;

public:
	Bomb();

	bool& is_exploding();
};
