#include "bomb.hpp"

/* Bomb class constructor */
Bomb::Bomb(std::pair<float,float> coords)
{
	this->_radius = EXPLOSION_RADIUS;
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


/* Returns _is_exploding, which indicates that the bomb object now represents an explosion */
bool& Bomb::is_exploding()
{
	return this->_is_exploding;
}
