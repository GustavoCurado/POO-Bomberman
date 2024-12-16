#include <cstdlib>
#include <ctime>
#include <cmath>
#include <chrono>
#include <thread>
#include <algorithm>
#include <stdexcept>
#include "game.hpp"

//Initializes some member variables
void Game::init_members()
{
	this->_window = nullptr;
	_map.load_map_data(MAP_FILENAME);
}


/* Initializes the game window */
void Game::init_window(int width, int height)
{
	this->_window = new sf::RenderWindow(sf::VideoMode(width, height), "Bomberman");
	this->_window->setFramerateLimit(60);
}


/* Game class constructor */
Game::Game()
{
	//Sets up RNG for rand() function
	srand(time(NULL));

	init_members();

	//Loads textures from sprite files located in the "sprites" folder
	if (!this->_field.loadFromFile(FIELD_FILENAME, sf::IntRect(0, 0, FIELD_WIDTH, FIELD_HEIGHT)))
	{
		throw std::runtime_error(std::string("Error: Could not open ") + FIELD_FILENAME + std::string("."));
	}
	if (!this->_spritesheet.loadFromFile(SPRITESHEET_FILENAME))
	{
		throw std::runtime_error(std::string("Error: Could not open ") + SPRITESHEET_FILENAME + std::string("."));
	}

	init_window(FIELD_WIDTH, FIELD_HEIGHT);	

	//Traverses the map, finds the player's location and the enemies',
	//get their coordinates and erase them from the map, since the map
	//is only supposed to store their location initially
	for (int i = 0; i < FIELD_BLOCK_HEIGHT; i++)
	{
		for (int j = 0; j < FIELD_BLOCK_WIDTH; j++)
		{
			if (_map[i][j] == M_CHAR)
			{
				_map[i][j] = ' ';
				_man.set_coordinates(std::pair(j*BLOCK_SIZE, i*BLOCK_SIZE));
			}
			else if	(_map[i][j] == E_CHAR)
			{
				_map[i][j] = ' ';
				
				Enemy enemy;
				enemy.set_coordinates(std::pair(j*BLOCK_SIZE, i*BLOCK_SIZE));
				_enemies.push_back(enemy);
			}
		}
	}
}


/* Game class destructor */
Game::~Game()
{
	delete this->_window;
}


/* Orders an entity to move in a certain direction    *
 * (given by the "keycode") if the path isn't blocked */
void Game::command_movement(Entity& entity, sf::Keyboard::Key keycode)
{
	int neighboring_i;
	int neighboring_j;
	int new_i;
	int new_j;
	bool is_empty;
	bool is_far_enough;
	bool is_centered;

	//*is_empty: true if the neighboring space in the map is empty (has no blocks or bombs)
	//*is_far_enough: true if the bomberman is far enough so that it's sprite won't overlap with a wall when it moves
	//*is_centered: true if the bomberman is in the center of one of the tile's axis before moving through their other axis

	//Movement is only possible if the Entity can move in a way in which its
	//sprite doesn't overlap with the sprite of one of the walls in the map.
	//The booleans values being checked prevent that from happening.
	switch (keycode)
	{
	case sf::Keyboard::Up:
		neighboring_i = (entity.y() - BLOCK_SIZE) / BLOCK_SIZE;
		neighboring_j = entity.x() / BLOCK_SIZE;
		is_empty = (this->_map[neighboring_i][neighboring_j] == G_CHAR);

		new_i = (entity.y() - entity.speed()) / BLOCK_SIZE;
		is_far_enough = (new_i != neighboring_i);

		is_centered = ((int)entity.x() % BLOCK_SIZE == 0);

		if (is_centered && (is_empty || is_far_enough))
			entity.move_up();

		entity.steps_counter()++;

		break;
	case sf::Keyboard::Down:
		neighboring_i = (entity.y() + BLOCK_SIZE) / BLOCK_SIZE;
		neighboring_j = entity.x() / BLOCK_SIZE;
		is_empty = (this->_map[neighboring_i][neighboring_j] == G_CHAR);

		is_centered = ((int)entity.x() % BLOCK_SIZE == 0);

		if (is_centered && is_empty)
			entity.move_down();

		entity.steps_counter()++;

		break;
	case sf::Keyboard::Left:
		neighboring_i = entity.y() / BLOCK_SIZE;
		neighboring_j = (entity.x() - BLOCK_SIZE) / BLOCK_SIZE;
		is_empty = (this->_map[neighboring_i][neighboring_j] == G_CHAR);

		new_j = (entity.x() - entity.speed()) / BLOCK_SIZE;
		is_far_enough = (new_j != neighboring_j);

		is_centered = ((int)entity.y() % BLOCK_SIZE == 0);

		if (is_centered && (is_empty || is_far_enough))
			entity.move_left();

		entity.steps_counter()++;

		break;
	case sf::Keyboard::Right:
		neighboring_i = entity.y() / BLOCK_SIZE;
		neighboring_j = (entity.x() + BLOCK_SIZE) / BLOCK_SIZE;
		is_empty = (this->_map[neighboring_i][neighboring_j] == G_CHAR);

		is_centered = ((int)entity.y() % BLOCK_SIZE == 0);

		if (is_centered && is_empty)
			entity.move_right();

		entity.steps_counter()++;

		break;
	default:
		break;
	}
}


/* Randomizes an enemy's movements */
void Game::random_movement(Enemy& enemy)
{
	//If the enemy hasn't moved (or tried to moved) 16 steps or more in a single
	//direction, it'll continue movng in the same direction. When it reaches this
	//limits, it'ĺl randomly change it's direction of movement
	if (enemy.steps_counter() >= 16)
	{
		enemy.steps_counter() = 0;
		int rand_n = rand() % 4;

		switch (rand_n)
		{
		case 0:
			this->command_movement(enemy, sf::Keyboard::Up);
			break;
		case 1:
			this->command_movement(enemy, sf::Keyboard::Down);
			break;
		case 2:
			this->command_movement(enemy, sf::Keyboard::Left);
			break;
		case 3:
			this->command_movement(enemy, sf::Keyboard::Right);
			break;
		}
	}
	else
	{
		this->command_movement(enemy, enemy.last_move());
	}
}


void Game::drop_bomb()
{
	int i = this->_man.y() / BLOCK_SIZE;
	int j = this->_man.x() / BLOCK_SIZE;

	int bomb_i;
	int bomb_j;

	switch (this->_man.last_move())
	{
	case sf::Keyboard::Up:
		bomb_i = i-1;
		bomb_j = j;
		break;
	case sf::Keyboard::Down:
		bomb_i = i+2;
		bomb_j = j;
		break;
	case sf::Keyboard::Left:
		bomb_i = i;
		bomb_j = j-1;
		break;
	case sf::Keyboard::Right:
		bomb_i = i;
		bomb_j = j+2;
		break;
	}

	char* bomberman_front = &this->_map[bomb_i][bomb_j]; //Not a NULL-terminated string, only a pointer to a single char

	if (*bomberman_front == G_CHAR)
	{
		*bomberman_front = B_CHAR;
		Bomb bomb(std::pair(bomb_j*BLOCK_SIZE, bomb_i*BLOCK_SIZE));
		this->_bombs.push_back(bomb);
	}
}


void Game::explode_bomb(Bomb& bomb)
{
	int i = bomb.y() / BLOCK_SIZE;
	int j = bomb.x() / BLOCK_SIZE;

	int upper_radius;
	int bottom_radius;
	int left_radius;
	int right_radius;

	bomb.is_exploding() = true;

	this->_map[i][j] = G_CHAR;

	for (upper_radius = 0; upper_radius < MAX_RADIUS; upper_radius++)
	{
		if (this->_map[i-upper_radius-1][j] == UW_CHAR)
			break;
	}
	for (bottom_radius = 0; bottom_radius < MAX_RADIUS; bottom_radius++)
	{
		if (this->_map[i+bottom_radius+1][j] == UW_CHAR)
			break;
	}
	for (left_radius = 0; left_radius < MAX_RADIUS; left_radius++)
	{
		if (this->_map[i][j-left_radius-1] == UW_CHAR)
			break;
	}
	for (right_radius = 0; right_radius < MAX_RADIUS; right_radius++)
	{
		if (this->_map[i][j+right_radius+1] == UW_CHAR)
			break;
	}

	bomb.upper_radius() = upper_radius;
	bomb.bottom_radius() = bottom_radius;
	bomb.left_radius() = left_radius;
	bomb.right_radius() = right_radius;

	bomb.explosion_time() = time(NULL);
}


void Game::end_explosion(Bomb& bomb)
{
	int i = bomb.y() / BLOCK_SIZE;
	int j = bomb.x() / BLOCK_SIZE;

	bomb.is_exploding() = false; //This line is probably unecessary

	for (int x = 1; x <= bomb.upper_radius(); x++)
	{
		this->_map[i-x][j] = G_CHAR;
	}
	for (int x = 1; x <= bomb.bottom_radius(); x++)
	{
		this->_map[i+x][j] = G_CHAR;
	}
	for (int x = 1; x <= bomb.left_radius(); x++)
	{
		this->_map[i][j-x] = G_CHAR;
	}
	for (int x = 1; x <= bomb.right_radius(); x++)
	{
		this->_map[i][j+x] = G_CHAR;
	}

	auto it = std::find(this->_bombs.begin(), this->_bombs.end(), bomb); 
	this->_bombs.erase(it);
}


/* Handles user inputs and commands through the keyboard and mouse */
void Game::update_events()
{
	while (this->_window->pollEvent(this->_event))
	{
		switch (this->_event.type)
		{
		case sf::Event::Closed:
			this->_window->close();
			break;
		case sf::Event::KeyPressed:
			if (this->_event.key.code == sf::Keyboard::Space)
				this->drop_bomb();
			else
				this->command_movement(this->_man, this->_event.key.code);

			break;
		default:
			break;
		}
	}
}


/* Ends the game because the bomberman was killed */
void Game::game_over()
{
	this->_man.is_dead() = true;

	this->render();

	std::this_thread::sleep_for(std::chrono::seconds(2));

	this->_window->close();
}


/* Checks if the player touched any of the enemies and, if they did, end the game */
void Game::death_check()
{
	bool they_touched;

	//*they_touched: true if the bomberman's and an enemy's sprite overlaped

	for (auto enemy : this->_enemies)
	{
		if (this->_man.x() == enemy.x())
			they_touched = (abs(this->_man.y() - enemy.y()) < BLOCK_SIZE);		
		else if (this->_man.y() == enemy.y())
			they_touched = (abs(this->_man.x() - enemy.x()) < BLOCK_SIZE);		
		else
			they_touched = false;

		if (they_touched)
			this->game_over();
	}
}


/* Updates the status and position of entities and objects in the game */
void Game::update()
{
	//If it's been more than SECONDS_UNTIL_EXPLOSION seconds
	//since the bomb was placed, it'll explode
	for (Bomb& bomb : this->_bombs)
	{
		if (!bomb.is_exploding() && time(NULL) >= bomb.bomb_time() + SECONDS_UNTIL_EXPLOSION)
			this->explode_bomb(bomb);
		else if (bomb.is_exploding() && time(NULL) >= bomb.explosion_time() + EXPLOSION_DURATION)
			this->end_explosion(bomb);
	}

	this->death_check();

	this->update_events();

	this->death_check();
	
	//Randomly moves all enemies in the game
	for (Enemy& enemy : this->_enemies)	
	{
		this->random_movement(enemy);
	}

	//There are three death checks because, if we only placed a death check after
	//both the Bomberman and his enemies moved, it's possible that, at their his
	//the Bomberman touches an enemy, but the enemy mover away from him on his turn,
	//making it so that the Bomberman doesn't die, even though he should have.
	this->death_check();
}


/* Draws the background and unbreakable walls */
void Game::draw_playfield()
{
	sf::Sprite field_sprite;
	field_sprite.setTexture(_field);
	this->_window->draw(field_sprite);
}


/* Draws all breakable walls */
void Game::draw_walls()
{
	for (int i = 0; i < FIELD_BLOCK_HEIGHT; i++)
	{
		for (int j = 0; j < FIELD_BLOCK_WIDTH; j++)
		{
			if (_map[i][j] == BW_CHAR)
			{
				sf::Sprite wall_sprite;
				wall_sprite.setTexture(this->_spritesheet);
				wall_sprite.setTextureRect(sf::IntRect(4*BLOCK_SIZE, 3*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
				wall_sprite.setPosition(j*BLOCK_SIZE, i*BLOCK_SIZE);

				this->_window->draw(wall_sprite);
			}
		}
	}
}


/* Draws the bomberman's sprite */
void Game::draw_bomberman()
{
	sf::Sprite man_sprite;
	man_sprite.setTexture(this->_spritesheet);
	
	if (!this->_man.is_dead())
	{
		switch (this->_man.last_move())
		{
		case sf::Keyboard::Up:
			man_sprite.setTextureRect(sf::IntRect(3*BLOCK_SIZE, 1*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE-1));
			break;
		case sf::Keyboard::Down:
			man_sprite.setTextureRect(sf::IntRect(5*BLOCK_SIZE, 0, BLOCK_SIZE-1, BLOCK_SIZE-1));
			break;
		case sf::Keyboard::Left:
			man_sprite.setTextureRect(sf::IntRect(0, 0, BLOCK_SIZE, BLOCK_SIZE-1));
			break;
		case sf::Keyboard::Right:
			man_sprite.setTextureRect(sf::IntRect(0, 1*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE-1));
			break;
		default:
			throw std::runtime_error("Error: Invalid movement for Bomberman.");
			break;
		}
	}
	else
	{
		man_sprite.setTextureRect(sf::IntRect(1*BLOCK_SIZE, 2*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
	}

	man_sprite.setPosition(this->_man.x(), this->_man.y());
	this->_window->draw(man_sprite);
}


/* Draws the enemies' sprites */
void Game::draw_enemies()
{
	for (auto enemy : this->_enemies)
	{
		sf::Sprite enemy_sprite;
		enemy_sprite.setTexture(this->_spritesheet);
		
		switch (enemy.last_move())
		{
		case sf::Keyboard::Up:
			enemy_sprite.setTextureRect(sf::IntRect(0, 20*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
			break;
		case sf::Keyboard::Down:
			enemy_sprite.setTextureRect(sf::IntRect(3*BLOCK_SIZE+1, 20*BLOCK_SIZE+1, BLOCK_SIZE, BLOCK_SIZE-1));
			break;
		case sf::Keyboard::Left:
			enemy_sprite.setTextureRect(sf::IntRect(5*BLOCK_SIZE, 20*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
			break;
		case sf::Keyboard::Right:
			enemy_sprite.setTextureRect(sf::IntRect(2*BLOCK_SIZE, 20*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
			break;
		default:
			throw std::runtime_error("Error: Invalid movement for Enemy.");
			break;
		}

		enemy_sprite.setPosition(enemy.x(), enemy.y());
		this->_window->draw(enemy_sprite);
	}
}


void Game::draw_normal_bomb(Bomb bomb)
{
	sf::Sprite bomb_sprite;
	bomb_sprite.setTexture(this->_spritesheet);

	bomb_sprite.setTextureRect(sf::IntRect(0, 3*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));

	bomb_sprite.setPosition(bomb.x(), bomb.y());
	this->_window->draw(bomb_sprite);
}


void Game::draw_exploding_bomb(Bomb bomb)
{
	sf::Sprite bomb_center_sprite;
	sf::Sprite bomb_upper_sprite;
	sf::Sprite bomb_bottom_sprite;
	sf::Sprite bomb_left_sprite;
	sf::Sprite bomb_right_sprite;
	sf::Sprite upper_end_sprite;
	sf::Sprite bottom_end_sprite;
	sf::Sprite left_end_sprite;
	sf::Sprite right_end_sprite;

	bomb_center_sprite.setTexture(this->_spritesheet);
	bomb_upper_sprite.setTexture(this->_spritesheet);
	bomb_bottom_sprite.setTexture(this->_spritesheet);
	bomb_left_sprite.setTexture(this->_spritesheet);
	bomb_right_sprite.setTexture(this->_spritesheet);
	upper_end_sprite.setTexture(this->_spritesheet);
	bottom_end_sprite.setTexture(this->_spritesheet);
	left_end_sprite.setTexture(this->_spritesheet);
	right_end_sprite.setTexture(this->_spritesheet);

	bomb_center_sprite.setTextureRect(sf::IntRect(7*BLOCK_SIZE, 11*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
	bomb_upper_sprite.setTextureRect(sf::IntRect(7*BLOCK_SIZE, 10*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
	bomb_bottom_sprite.setTextureRect(sf::IntRect(7*BLOCK_SIZE, 12*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
	bomb_left_sprite.setTextureRect(sf::IntRect(8*BLOCK_SIZE, 11*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
	bomb_right_sprite.setTextureRect(sf::IntRect(6*BLOCK_SIZE, 11*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
	upper_end_sprite.setTextureRect(sf::IntRect(7*BLOCK_SIZE, 9*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
	bottom_end_sprite.setTextureRect(sf::IntRect(7*BLOCK_SIZE, 13*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
	left_end_sprite.setTextureRect(sf::IntRect(5*BLOCK_SIZE, 11*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));
	right_end_sprite.setTextureRect(sf::IntRect(9*BLOCK_SIZE, 11*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));

	bomb_center_sprite.setPosition(bomb.x(), bomb.y());
	this->_window->draw(bomb_center_sprite);

	for (int i = 1; i <= bomb.upper_radius(); i++)
	{
		if (i != bomb.upper_radius())
		{
			bomb_upper_sprite.setPosition(bomb.x(), bomb.y()-i*BLOCK_SIZE);
			this->_window->draw(bomb_upper_sprite);
		}
		else
		{
			upper_end_sprite.setPosition(bomb.x(), bomb.y()-i*BLOCK_SIZE);
			this->_window->draw(upper_end_sprite);
		}
	}
	for (int i = 1; i <= bomb.bottom_radius(); i++)
	{
		if (i != bomb.bottom_radius())
		{
			bomb_bottom_sprite.setPosition(bomb.x(), bomb.y()+i*BLOCK_SIZE);
			this->_window->draw(bomb_bottom_sprite);
		}
		else
		{
			bottom_end_sprite.setPosition(bomb.x(), bomb.y()+i*BLOCK_SIZE);
			this->_window->draw(bottom_end_sprite);
		}
	}
	for (int i = 1; i <= bomb.left_radius(); i++)
	{
		if (i != bomb.left_radius())
		{
			bomb_left_sprite.setPosition(bomb.x()-i*BLOCK_SIZE, bomb.y());
			this->_window->draw(bomb_left_sprite);
		}
		else
		{
			left_end_sprite.setPosition(bomb.x()-i*BLOCK_SIZE, bomb.y());
			this->_window->draw(left_end_sprite);
		}
	}
	for (int i = 1; i <= bomb.right_radius(); i++)
	{
		if (i != bomb.right_radius())
		{
			bomb_right_sprite.setPosition(bomb.x()+i*BLOCK_SIZE, bomb.y());
			this->_window->draw(bomb_right_sprite);
		}
		else
		{
			right_end_sprite.setPosition(bomb.x()+i*BLOCK_SIZE, bomb.y());
			this->_window->draw(right_end_sprite);
		}
	}
}


void Game::draw_bombs()
{
	for (auto bomb : this->_bombs)
	{
		if (!bomb.is_exploding())
			this->draw_normal_bomb(bomb);
		else
			this->draw_exploding_bomb(bomb);
	}
}


/* Handles all graphic elements of the game */
void Game::render()
{
	this->_window->clear();

	this->draw_playfield();
	this->draw_walls();
	this->draw_enemies();
	this->draw_bombs();
	this->draw_bomberman();

	this->_window->display();
}


/* Checks if the window is still open */
bool Game::is_open() const
{
	return this->_window->isOpen();
}
