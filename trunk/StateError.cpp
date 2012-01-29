#include "StateError.h"

#include "Vector2di.h"
#include "GreySurfaceCache.h"
#include "gameDefines.h"

StateError::StateError()
{
    errorDisplay.loadFont(GAME_FONT,36);
    errorDisplay.setBoundaries(Vector2di(20,20),Vector2di(780,460));
    errorDisplay.setWrapping(true);
    errorDisplay.setRelativity(true);
    //errorDisplay.setAlignment(CENTRED);
    errorDisplay.setColour(RED);

    errorString = "An unknown error occurred, sorry about that!\n\
                    At lest the game did not crash...";

    bool fromCache;
    bg.loadFrames(SURFACE_CACHE->loadSurface("images/menu/error_bg_800_480.png"),1,1,0,0);
    bg.disableTransparentColour();
    bg.setPosition(0,0);
}

StateError::~StateError()
{
    cout << errorString << endl;
}

void StateError::userInput()
{
    input->update();

    if (input->isAny())
        setNextState(returnState);
}

void StateError::update()
{
    bg.update();
}

void StateError::render()
{
    bg.render();

    errorDisplay.setPosition(20,180);
    errorDisplay.print(errorString);
    errorDisplay.print("\n\n");
    errorDisplay.print("Press any key!");
}
