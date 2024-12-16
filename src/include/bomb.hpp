#pragma once

#include <ctime>
#include <utility>

#define MAX_RADIUS 3
#define SECONDS_UNTIL_EXPLOSION 5
#define EXPLOSION_DURATION 2

class Bomb
{
private:
	int _radius;
	bool _is_exploding;
	time_t _bomb_time;
	time_t _explosion_time;
	std::pair<float,float> _coords;

	int _upper_radius;
	int _bottom_radius;
	int _left_radius;
	int _right_radius;

public:
	Bomb(std::pair<float,float> coords);

	//Acessors
	float x();
	float y();
	time_t bomb_time();
	time_t& explosion_time();
	bool& is_exploding();
	int& upper_radius();
	int& bottom_radius();
	int& left_radius();
	int& right_radius();

	friend bool operator==(const Bomb& bomb1, const Bomb& bomb2);
};
