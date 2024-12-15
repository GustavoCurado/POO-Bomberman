#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include "map.hpp"
#include "bomberman.hpp"
#include "enemy.hpp"

#define BLOCK_SIZE 32

#define FIELD_FILENAME "sprites/playfield.png"
#define SPRITESHEET_FILENAME "sprites/spritesheet.png"

#define FIELD_WIDTH 992
#define FIELD_HEIGHT 416
#define FIELD_BLOCK_WIDTH FIELD_WIDTH/BLOCK_SIZE
#define FIELD_BLOCK_HEIGHT FIELD_HEIGHT/BLOCK_SIZE

class Game
{
private:
	sf::RenderWindow* _window;
	sf::Event _event;
	sf::Texture _field;
	sf::Texture _spritesheet;
	Map _map;
	Bomberman _man;
	std::vector<Enemy> _enemies;

	void init_members();
	void init_window(int width, int height);
	void command_movement(Entity& entity, sf::Keyboard::Key keycode);
	void random_movement(Enemy& enemy);
	void draw_playfield();
	void draw_walls();
	void draw_bomberman();
	void draw_enemies();
	void update_events();

public:
	Game();
	~Game();

	void update();
	void render();
	bool is_open() const;
};
