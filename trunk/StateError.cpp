/*
	Greyout - a colourful platformer about love

	Greyout is Copyright (c)2011-2014 Janek Schäfer

	This file is part of Greyout.

    Greyout is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

	Greyout is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

	Please direct any feedback, questions or comments to
	Janek Schäfer (foxblock), foxblock_at_gmail_dot_com
*/

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

    bg.loadFrames(SURFACE_CACHE->loadSurface("images/menu/error_bg_800_480.png"),1,1,0,0);
    bg.disableTransparentColour();
    bg.setPosition(0,0);
}

StateError::~StateError()
{
    printf("ERROR: %s\n",errorString.c_str());
}

void StateError::userInput()
{
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
