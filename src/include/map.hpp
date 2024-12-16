#pragma once

#include <string>
#include <vector>
#include <utility>

#define MAP_FILENAME "maps/map.txt"

#define UW_CHAR '#' //Unbreakable Wall
#define BW_CHAR '@' //Breakable wall
#define G_CHAR ' '  //Ground
#define B_CHAR 'O'  //Bomb
#define M_CHAR 'M'  //Bomberman
#define E_CHAR 'E'  //Enemy

#define C_CHAR 'X' //Center of the explosion
#define U_CHAR '^' //Upper part of the explosion
#define D_CHAR 'v' //Bottom part of the explosion
#define L_CHAR '<' //Left part of the explosion
#define R_CHAR '>' //Right part of the explosion


/* Stores the position of the walls and bombs sprites, and, at the game's initialization, *
 * stores the initial positions of the enemies and of the player                          */
class Map
{
private:
	std::vector<std::vector<char>> _matrix;

public:
	void load_map_data(std::string filename);
	std::vector<char>& operator[](size_t row);
};

