#include "bomberman.hpp"

/* Bomberman class constructor */
Bomberman::Bomberman()
{
	this->_last_move = sf::Keyboard::Down;
	this->_is_dead = false;
	this->_speed = BOMBERMAN_SPEED;
	this->_steps_counter = 0;   //_steps_counter isn't used here, but I need to initialize it
}



/*Returns _is_dead_, which is true if something killed the Bomberman */
bool& Bomberman::is_dead()
{
	return this->_is_dead;
}
