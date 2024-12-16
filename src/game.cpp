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

		entity.steps_counter()++; //Counts how many steps an enemy moved in single direction

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


/* Places a bomb on front of the Bomberman */
void Game::drop_bomb()
{
	int i = this->_man.y() / BLOCK_SIZE; //Position on _map analogous to the Bomberman's position
	int j = this->_man.x() / BLOCK_SIZE;

	int bomb_i;
	int bomb_j;

	//Determines where in the map the bomb will be dropped based
	//on the direction in which the bomberman is looking at
	switch (this->_man.last_move())
	{
	case sf::Keyboard::Up:
		bomb_i = i-1;
		bomb_j = j;
		break;
	case sf::Keyboard::Down:
		if ((int)this->_man.y() % BLOCK_SIZE == 0) //Checks if the Bomberman is in the center of the _map tile
		{
			bomb_i = i+1;
			bomb_j = j;
		}
		else
		{
			bomb_i = i+2;
			bomb_j = j;
		}

		break;
	case sf::Keyboard::Left:
		bomb_i = i;
		bomb_j = j-1;
		break;
	case sf::Keyboard::Right:
		if ((int)this->_man.x() % BLOCK_SIZE == 0)
		{
			bomb_i = i;
			bomb_j = j+1;
		}
		else
		{
			bomb_i = i;
			bomb_j = j+2;
		}
		break;
	}

	char* bomberman_front = &this->_map[bomb_i][bomb_j]; //Not a NULL-terminated string, only a pointer to a single char

	//If there are no walls that prevent the bomb from being placed there,
	//the bomb is placed there in the _map and inserted in the vector of bombs
	if (*bomberman_front == G_CHAR)
	{
		*bomberman_front = B_CHAR;
		Bomb bomb(std::pair(bomb_j*BLOCK_SIZE, bomb_i*BLOCK_SIZE));
		this->_bombs.push_back(bomb);
	}
}


void Game::explode_bomb(Bomb& bomb)
{
	int i = bomb.y() / BLOCK_SIZE;  //Position on _map analogous to the Bomberman's position
	int j = bomb.x() / BLOCK_SIZE;

	int upper_radius;
	int bottom_radius;
	int left_radius;
	int right_radius;

	bomb.is_exploding() = true;

	//Removes the non-exploded bomb character from the map
	this->_map[i][j] = G_CHAR;

	//These for-loops find the radius of the explosion by verifying if there are
	//unbreakable walls (UW_CHAR) in the surroundings of the bomb. If there are
	//the radius in this certain direction will be smaller since the wall stops
	//the propagation of the explosion
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

	//Stores the radii in the bomb class object
	bomb.upper_radius() = upper_radius;
	bomb.bottom_radius() = bottom_radius;
	bomb.left_radius() = left_radius;
	bomb.right_radius() = right_radius;

	//Stores the timestamp of the moment in which the bomb exploded;
	bomb.explosion_time() = time(NULL);
}


void Game::end_explosion(Bomb& bomb)
{
	int i = bomb.y() / BLOCK_SIZE;
	int j = bomb.x() / BLOCK_SIZE;

	bomb.is_exploding() = false; //This line is probably unecessary, but I won't risk it

	//Fills the places in the map affected by the explosion with G_CHAR (the empty ground tile)
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

	//Erase the exploded bomb from the vector of bombs
	auto it = std::find(this->_bombs.begin(), this->_bombs.end(), bomb); 
	this->_bombs.erase(it);
}



/* Ends the game after defeating all enemies */
void Game::victory()
{
	this->render();

	//Wait 
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	//Does a clockwise spinning Bomberman animation
	for (int i = 0; i < 5; i++)
	{
		this->_man.last_move() = sf::Keyboard::Down;
		this->render();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		this->_man.last_move() = sf::Keyboard::Left;
		this->render();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		this->_man.last_move() = sf::Keyboard::Up;
		this->render();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		this->_man.last_move() = sf::Keyboard::Right;
		this->render();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	this->_window->close();
}


/* Ends the game because the bomberman was killed */
void Game::game_over()
{
	this->_man.is_dead() = true;

	//Shows the dead Bomberman sprite
	this->render();

	//Waits 2 seconds
	std::this_thread::sleep_for(std::chrono::seconds(2));

	this->_window->close();
}


/* Checks if the player touched any of the enemies or bombs and, if *
 * they did, end the game. Also checks if an enemy touched the bomb */
void Game::death_check()
{
	std::vector<Enemy> killed_enemies;

	bool man_touched_enemy;
	bool man_touched_bomb;
	bool bomb_touched_enemy;

	//*man_touched_enemy: true if the bomberman's and an enemy's sprite overlaped
	//*man_touched_bomb: true if the bomberman's and a bomb's sprite overlaped
	//*bomb_touched_enemy: true if a bomb's and an enemy's sprite overlaped

	for (auto enemy : this->_enemies)
	{
		//Variable initialization
		man_touched_enemy = false;

		//If the bomberman and an enemy share neither of the two axis, there's no possibility
		//of them touching. If they do share an axis, it checks if the distance between them
		//in that axis is smaller than a certain limit, killing the bomberman if it is. These
		//checks are also made for bomb_touched_enemy and man_touched_bomb

		//Checks if bomberman touched an enemy
		if (this->_man.x() == enemy.x())
			man_touched_enemy = (abs(this->_man.y() - enemy.y()) < BLOCK_SIZE);		
		else if (this->_man.y() == enemy.y())
			man_touched_enemy = (abs(this->_man.x() - enemy.x()) < BLOCK_SIZE);		

		//The game ends if the bomberman collides with an enemy
		if (man_touched_enemy)
			this->game_over();

		//Checks every bomb to see if the explosion caught an enemy
		for (auto bomb : this->_bombs)
		{
			//If the bomb isn't exploding, it isn't going to kill anything
			if (!bomb.is_exploding())
				continue;

			//Variable initialization
			bomb_touched_enemy = false;

			if (bomb.x() == enemy.x())
			{
				//if the bomb is below the enemy
				if (bomb.y() > enemy.y())
					bomb_touched_enemy = (abs(bomb.y() - enemy.y()) < (1+bomb.upper_radius())*BLOCK_SIZE);		
				else
					bomb_touched_enemy = (abs(bomb.y() - enemy.y()) < (1+bomb.bottom_radius())*BLOCK_SIZE);
			}
			else if (bomb.y() == enemy.y())
			{
				//if the bomb is on the enemy's right side
				if (bomb.x() > enemy.x())
					bomb_touched_enemy = (abs(bomb.x() - enemy.x()) < (1+bomb.left_radius())*BLOCK_SIZE);		
				else
					bomb_touched_enemy = (abs(bomb.x() - enemy.x()) < (1+bomb.right_radius())*BLOCK_SIZE);		
			}

			//If the explosion caught an enemy, they must be killed later
			if (bomb_touched_enemy)
				killed_enemies.push_back(enemy);
		}
	}

	//Checks every bomb to see if the explosion caught the bomberman
	for (auto bomb : this->_bombs)
	{
		//Variable initialization
		man_touched_bomb = false;

		if (!bomb.is_exploding())
			continue;

		//Here, the limit of distance between the center of the explosion and the bomberman
		//is determined by the position of the bomberman relative to the bomb and the radius
		//of explosion in that direction

		if (this->_man.x() == bomb.x())
		{
			if (bomb.y() > this->_man.y())
				man_touched_bomb = (abs(bomb.y() - this->_man.y()) < (1+bomb.upper_radius())*BLOCK_SIZE);		
			else
				man_touched_bomb = (abs(bomb.y() - this->_man.y()) < (1+bomb.bottom_radius())*BLOCK_SIZE);
		}
		else if (this->_man.y() == bomb.y())
		{
			if (bomb.x() > this->_man.x())
				man_touched_bomb = (abs(bomb.x() - this->_man.x()) < (1+bomb.left_radius())*BLOCK_SIZE);		
			else
				man_touched_bomb = (abs(bomb.x() - this->_man.x()) < (1+bomb.right_radius())*BLOCK_SIZE);		
		}

		//The game ends if the explosion catches the bomberman
		if (man_touched_bomb)
			this->game_over();
	}

	//Goes through the list of enemies that were killed and erase them from the vector of enemies
	for (auto enemy : killed_enemies)
	{
		auto it2 = std::find(this->_enemies.begin(), this->_enemies.end(), enemy);
		this->_enemies.erase(it2);
	}
}


/* Handles user inputs and commands through the keyboard and mouse */
void Game::update_events()
{
	while (this->_window->pollEvent(this->_event))
	{
		switch (this->_event.type)
		{
		case sf::Event::Closed:  //Closes window if ordered to
			this->_window->close();
			break;
		case sf::Event::KeyPressed:  //If any key is pressed
			if (this->_event.key.code == sf::Keyboard::Space) //If it's the Spacebar, drop a bomb
				this->drop_bomb();
			else
				this->command_movement(this->_man, this->_event.key.code); //If it's any other key, try to move the bomberman

			break;
		default:
			break;
		}
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

	//Game is won if all enemies are defeated
	if (this->_enemies.empty())
		this->victory();
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
	//The location of breakable walls is stored in the _map class object, so the loops iterate through it
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


/* Draws a bomb that is yet to explode */
void Game::draw_normal_bomb(Bomb bomb)
{
	sf::Sprite bomb_sprite;
	bomb_sprite.setTexture(this->_spritesheet);

	bomb_sprite.setTextureRect(sf::IntRect(0, 3*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE));

	bomb_sprite.setPosition(bomb.x(), bomb.y());
	this->_window->draw(bomb_sprite);
}


/* Draws the explosion sprites of an exploding bomb */
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

	// -> bomb_center_sprite: The explosion sprite that replaces the tile where the bomb itself is
	// -> ////_end_sprite: The explosion sprite located at the tips of the cross that the explosion forms
	// -> bomb_///_sprite: The explosion sprite located at the middle of the "arms" of the cross formed by the explosion

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


/* Handles the drawing of bombs and explosions */
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
