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
// TODO: Add settings menu to pause menu (and maybe main menu?) and implement settings (settings class? also draw and control settings here)
//		- move sound and music volume
//		- draw pattern (off, lines, shaded, full)
//		- particle density (off, few, many) -> could affect particle creation factor or simply limit in Level::addParticle (or both)
//		- draw fps (on, off)
//		- debug controls (on, off) ? (unit selection)
//		- write fps (on, off) -> write to file every second
//		- draw links (on, off) -> simply limit Level::addLink
//		boolean settings as checkboxes white outline with black fill (off) or white fill (on)
// TODO: Improve creation and destruction of links (better checks in hitUnit and removal in update - since hitUnit is not called when no hit occurs)
// TODO: Move handling of key units to exit class (to allow multiple keys per exit and draw links, checks performed simply by destruction of key - possible, race conditions?)
// TODO: Option for swapControl to cycle through the player vector instead of toggling two groups (level flag?)

// TODO: Level with three levels: black in lower level, one pit with spikes, a few black boxes on upper level, white in second level with white box to form shaft for the boxes
// TODO: Checkerboard level with fading blocks, a few keep colour, a few fade to red (tried that, kinda failed...)

// Greyout colours (RGB):
// Green:   50 217  54
// Grey:   147 149 152
// Red:    255   0   0
// White:  255 255 255
// Black:    0   0   0
// Orange: 255 153   0
// Blue:     0  51 255

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
