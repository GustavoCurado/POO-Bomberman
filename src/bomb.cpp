#include "bomb.hpp"

/* Bomb class constructor */
Bomb::Bomb(std::pair<float,float> coords)
{
	this->_radius = MAX_RADIUS;
	this->_is_exploding = false;
	this->_bomb_time = time(NULL);
	this->_coords = coords;
}


float Bomb::x()
{
	return this->_coords.first;
}


float Bomb::y()
{
	return this->_coords.second;
}


/* Returns _bomb_time, which marks the moment in which the bomb was placed on the ground */
time_t Bomb::bomb_time()
{
	return this->_bomb_time;
}


/* Returns _explosion_time, which marks the moment in which the bomb exploded */
time_t& Bomb::explosion_time()
{
	return this->_explosion_time;
}


/* Returns _is_exploding, which indicates that the bomb object now represents an explosion */
bool& Bomb::is_exploding()
{
	return this->_is_exploding;
}


int& Bomb::upper_radius()
{
	return this->_upper_radius;
}


int& Bomb::bottom_radius()
{
	return this->_bottom_radius;
}


int& Bomb::left_radius()
{
	return this->_left_radius;
}


int& Bomb::right_radius()
{
	return this->_right_radius;
}


/* Operator used by the std::find function */
bool operator==(const Bomb& bomb1, const Bomb& bomb2)
{
	return bomb1._coords.first == bomb2._coords.first && bomb1._coords.second == bomb2._coords.second;
}
