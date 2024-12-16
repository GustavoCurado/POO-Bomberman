#include "entity.hpp"

/* Returns the x-coordinate of the entity */
float Entity::x()
{
	return this->_coords.first;
}


/* Returns the y-coordinate of the entity */
float Entity::y()
{
	return this->_coords.second;
}


/* Returns _speed, that is, the amount of pixels the entity moves per action */
float Entity::speed()
{
	return this->_speed;
}


/* Returns _steps_counter, which counts the amount of times the entity       *
 * moved (or tried to move) in its current direction (only used for enemies) */
int& Entity::steps_counter()
{
	return this->_steps_counter;
}


/* Returns _last_move, which is the last direction in which the entity moved to */
sf::Keyboard::Key& Entity::last_move()
{
	return this->_last_move;
}


/* Initializes the entity's coordinates */
void Entity::set_coordinates(std::pair<float,float> coords)
{
	this->_coords = coords;
}


/* Moves the entity up "_speed" number of pixels and updates _last_movement */
void Entity::move_up()
{
	this->_last_move = sf::Keyboard::Up;
	this->_coords.second -= this->_speed;
}


void Entity::move_down()
{
	this->_last_move = sf::Keyboard::Down;
	this->_coords.second += this->_speed;
}


void Entity::move_left()
{
	this->_last_move = sf::Keyboard::Left;
	this->_coords.first -= this->_speed;
}


void Entity::move_right()
{
	this->_last_move = sf::Keyboard::Right;
	this->_coords.first += this->_speed;
}
