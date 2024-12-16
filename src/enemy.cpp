#include "enemy.hpp"

/* Enemy class constructor */
Enemy::Enemy()
{
	this->_last_move = sf::Keyboard::Down;
	this->_speed = ENEMY_SPEED;
	this->_steps_counter = 0;
}


/* Operator used by the std::find function */
bool operator==(const Enemy& enemy1, const Enemy& enemy2)
{
	return enemy1._coords.first == enemy2._coords.first && enemy1._coords.second == enemy2._coords.second;
}
