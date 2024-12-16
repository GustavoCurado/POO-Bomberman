#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <utility>

class Entity
{
protected:
	std::pair<float,float> _coords;
	float _speed;
	int _steps_counter; 
	sf::Keyboard::Key _last_move;

public:
	//Acessors
	float x();
	float y();
	bool is_dying();
	float speed();
	int& steps_counter();
	sf::Keyboard::Key last_move();

	void set_coordinates(std::pair<float,float> coords);
	void move_up();
	void move_down();
	void move_left();
	void move_right();
};
