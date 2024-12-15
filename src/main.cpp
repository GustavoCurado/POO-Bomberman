#include "game.hpp"

int main()
{
	Game game;

	while (game.is_open())
	{
		game.update();
		game.render();
	}

	return 0;
}
