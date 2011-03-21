#include "MyGame.h"

int main(int argc, char** argv)
{
    Engine* game = NULL;
	ErrorHandler error;

	//	Setup game engine
	game = new MyGame;

	cout << error.getErrorString(game->argHandler(argc,argv));
	cout << error.getErrorString(game->penjinInit());

	while(game->stateLoop());	//	Perform main loop until shutdown
	cout << error.getErrorString(PENJIN_SHUTDOWN);

	//	Tidy up
    if(game)
    {
        delete game;
        game = NULL;
    }
    cout << error.getErrorString(PENJIN_GOODBYE);
    SDL_Quit();		//	Shut down SDL tidyly
	return 0;	//	Normal program termination.
}
