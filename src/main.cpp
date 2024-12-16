#include "game.hpp"

int main()
{
	Game game;

	while (game.is_open())
	{
		game.render();
		game.update();
	}

	return 0;
}
