#include "MyGame.h"

// DONE: Story
// TODO: "Connected" level type
// TODO: More levels, organic levels, more assets (trees, gears)
// TODO: Unit types ((DONE)temporary platforms, (DONE)timed platforms, (DONE)keys, lasers)
// DONE: Time-trial mode
// DONE: Change buttons for menu
// TODO: Show chapter/level names in selection
// DONE: Set last chapter as active chapter for "Start game"
// DONE: Randomize particle decay
// DONE: Switch to 60fps
// DONE: Fix boxes not falling through gaps (slower push speed)
// DONE: Fix animations when pushing
// DONE: Fix not being able to push boxes from the other side of the screen
// DONE: Fix menu selector speed
// TODO: Test with notaz' improved SDL
// DONE: Add reset functions to PushableBox,BaseTrigger,etc. to always get back to default values
// TODO: Experiment with vector::capacity on unit and especially particle vectors (also check vectors used in physics, whose size can be assumed)

// TODO: Level with three levels: black in lower level, one pit with spikes, a few black boxes on upper level, white in second level with white box to form shaft for the boxes
// TODO: Checkerboard level with fading blocks, a few keep colour, a few fade to red

int main(int argc, char** argv)
{
    Engine* game = NULL;
	ErrorHandler error;

	//	Setup game engine
	game = new MyGame;

    if(game)
    {
        cout << error.getErrorString(game->argHandler(argc,argv));
        cout << error.getErrorString(game->penjinInit());

		GFX::showCursor(true);

        while(game->stateLoop());	//	Perform main loop until shutdown

        cout << error.getErrorString(PENJIN_SHUTDOWN);

	//	Tidy up
        delete game;
        game = NULL;
    }
    else
    {
        cout << "CRITICAL ERROR: Failed to create game class!" << endl;
    }

    cout << error.getErrorString(PENJIN_GOODBYE);
    SDL_Quit();		//	Shut down SDL tidyly
	return 0;	//	Normal program termination.
}
