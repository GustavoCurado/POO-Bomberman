#include <fstream>
#include <stdexcept>
#include "map.hpp"


/* Loads the initial position for the sprites from a text file */
void Map::load_map_data(std::string filename)
{
	std::ifstream file(filename);
	if (!file.is_open())
	{
		throw std::runtime_error(std::string("Error: Coud not open ") + MAP_FILENAME + std::string("."));
	}

	std::string line;

	while (std::getline(file, line))
	{
		std::vector<char> line_vector;
		line_vector.insert(line_vector.end(), line.begin(), line.end());

		this->_matrix.push_back(line_vector);
	}

	file.close();
}


/* Overloads the [] operator so that the elements of the matrix can be accessed using it */
std::vector<char>& Map::operator[](size_t row)
{
	return this->_matrix[row];
}
