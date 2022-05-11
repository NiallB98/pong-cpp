#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")									// Hide console when running compiled version
#include "Game.h"

int main()
{
	srand(time(NULL));

	Game game;

	while (game.running)
	{
		game.update();
		game.render();
	}

	return 0;
}