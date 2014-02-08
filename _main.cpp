#include "MyGame.h"

// DONE: Story
// TODO: "Connected" level type
// TODO: More levels, organic levels, more assets (trees, (DONE)gears)
// TODO: Unit types (temporary platforms, (DONE)timed platforms, (DONE)keys, lasers)
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
// DONE: Experiment with vector::capacity on unit and especially particle vectors (also check vectors used in physics, whose size can be assumed)
// DONE: implement rest of settings class (controls, mouse, arrows on lists and sliders)
// DONE: Improve creation and destruction of links (better checks in hitUnit and removal in update - since hitUnit is not called when no hit occurs)
// DONE: Move handling of key units to exit class (to allow multiple keys per exit and draw links, checks performed simply by destruction of key - possible, race conditions?)
// DONE: Option for swapControl to cycle through the player vector instead of toggling two groups (level flag?)
// DONE: Consitent button usage (right click -> start- / cancel- button)
// DONE: Handling of settings in MyGame (via Settings::show function)
// DONE: Drawing of FPS in MyGame
// TODO: More warnings on unit::load functions
// DONE: Triggers take hitting unit as target when target vector is empty
// TODO: BaseTrigger, vector with hitting units and timeout for each unit separately
// DONE: Fix arrow draw mode on scrolling levels
// DONE: Better arrows (contrast on white bg)
// DONE: Reposition arrows on settings menu (centred with clickable area)
// TODO: Fades between menus (fade out on exit, fade in on enter)
// DONE: Alternate number format for times in level file: XXf (XX number of frames instead of ms - default)
// TODO: Change all time values to frames (?) --> Timer class based on frames (at least be consistent about it!)
// TODO: Count-Down on speedrun (full chapter runs)
// DONE: Local, (static) function in BaseUnit to load often used types (colour, target list, etc.)
// DONE: Other format for colour declaration in level files: XXXrXXXgXXXb, also hex: YYYYYYx
// TODO: Add setting for camera movement (smooth following, looking ahead)
// DONE: Add debug output for colour under cursor
// DONE: Colour mapping to surface format after loading from level file
// DONE: Debug function to advance game logic by single frames
// TODO: Avoid converting back and forth of colour values in collision functions by saving collision colours in SDL uint (watch for bit depth!)

// TODO: Level with three levels: black in lower level, one pit with spikes, a few black boxes on upper level, white in second level with white box to form shaft for the boxes
// TODO: Checkerboard level with fading blocks, a few keep colour, a few fade to red (tried that, kinda failed...)
// TODO: Blocks that fall when stepped on (purely for the effect, should not affect gameplay much)
// TODO: More levels with grey, it's what makes this game interesting - the link between two characters in two different worlds

// Greyout colours:
//            R   G   B      int
// Green:    50 217  54  3594546 - 3332406
// Grey:    147 149 152  9999763 - 9672088
// Red:     255   0   0 16711680
// White:   255 255 255 16777215
// Black:     0   0   0        0
// Orange:  255 153   0    39423 - 16750848
// LOrange: 255 220 168 11723007 - 16768168
// Blue:      0  51 255 16724736 - 13311
// LBlue:   178 193 255 16761266 - 11715071
// B+O Mix: 216 206 212 13946584 - 14208724

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
        return 666;
    }

    cout << error.getErrorString(PENJIN_GOODBYE);
    SDL_Quit();		//	Shut down SDL tidyly
	return 0;	//	Normal program termination.
}
