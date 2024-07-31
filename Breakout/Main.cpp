#include "Game.h"

int main()
{
	Game* app = new Game();
	app->Run();
		
	delete app;
	
    return 0;
}