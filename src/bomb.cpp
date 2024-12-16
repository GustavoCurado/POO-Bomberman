#include "bomb.hpp"

/* Bomb class constructor */
Bomb::Bomb()
{
	this->_radius = EXPLOSION_RADIUS;
	this->_is_exploding = false;
}


/* Returns _is_exploding, which indicates that the bomb object now represents an explosion */
bool& Bomb::is_exploding()
{
	return this->_is_exploding;
}
