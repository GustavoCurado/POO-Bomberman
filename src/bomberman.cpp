#include "bomberman.hpp"

/* Bomberman class constructor */
Bomberman::Bomberman()
{
	this->_last_move = sf::Keyboard::Down;
	this->_is_dying = false;
	this->_speed = BOMBERMAN_SPEED;
	this->_steps_counter = 0;   //_steps_counter isn't used here, but I need to initialize it
}
