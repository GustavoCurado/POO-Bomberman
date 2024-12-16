#pragma once

#include <ctime>
#include <utility>

#define EXPLOSION_RADIUS 3
#define SECONDS_UNTIL_EXPLOSION 5

class Bomb
{
private:
	int _radius;
	bool _is_exploding;
	time_t _bomb_time;
	std::pair<float,float> _coords;

public:
	Bomb(std::pair<float,float> coords);

	//Acessors
	float x();
	float y();
	time_t bomb_time();
	bool& is_exploding();
};
