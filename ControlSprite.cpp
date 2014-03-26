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

#include "ControlSprite.h"

ControlSprite::ControlSprite(Level* newParent) : BaseUnit(newParent)
{
    flags.addFlag(ufNoMapCollision);
    flags.addFlag(ufNoGravity);
    flags.addFlag(ufInvincible);
    unitCollisionMode = 0;
}

ControlSprite::~ControlSprite()
{
    //
}

///---public---

bool ControlSprite::load(list<PARAMETER_TYPE >& params)
{
    bool result = BaseUnit::load(params);

    if (imageOverwrite[0] == 0)
    {
		imageOverwrite = "images/units/controls.png";
    }
    else // clear sprites loaded by BaseUnit
    {
        for (map<string,AnimatedSprite*>::iterator I = states.begin(); I != states.end(); ++I)
        {
            delete I->second;
        }
        states.clear();
    }
    SDL_Surface* surf = getSurface(imageOverwrite);

	// Spritesheet (3x2): first row: Handhelds, second row: PC
	#ifdef PLATFORM_PC
	int initSkip = 3;
	#else
	int initSkip = 0;
	#endif
    loadFrames(surf,initSkip++,1,false,"jump");
    loadFrames(surf,initSkip++,1,false,"swap");
    loadFrames(surf,initSkip++,1,false,"suicide");

    setSpriteState(startingState,true);

    return result;
}

///---protected---


///---private---

AnimatedSprite* ControlSprite::loadFrames(SDL_Surface* const surf, CRint skip, CRint num, CRbool loop, CRstring state)
{
    AnimatedSprite* temp = new AnimatedSprite;
    temp->loadFrames(surf,3,2,skip,num);
    temp->setTransparentColour(MAGENTA);
    temp->setFrameRate(DECI_SECONDS);
    temp->setLooping(loop);
    states[state] = temp;
    return temp;
}
