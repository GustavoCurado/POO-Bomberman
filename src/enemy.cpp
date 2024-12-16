#include "enemy.hpp"

/* Enemy class constructor */
Enemy::Enemy()
{
	this->_last_move = sf::Keyboard::Down;
	this->_speed = ENEMY_SPEED;
	this->_steps_counter = 0;
}
