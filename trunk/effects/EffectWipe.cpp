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

#include "EffectWipe.h"

EffectWipe::EffectWipe(CRint time, const SimpleDirection& startingDirection, const Colour& wipeColour, CRbool inverted) : BaseEffect()
{
    direction = startingDirection;
    isInverted = inverted;
    int temp = 0;

    if (isInverted)
    {
        rect.setDimensions(GFX::getXResolution(),GFX::getYResolution());
        rect.setPosition(0,0);
        if (direction.xDirection() != 0)
        {
            temp = direction.xDirection();
        }
        else
        {
            temp = direction.yDirection();
        }
    }
    else
    {
        if (direction.xDirection() != 0)
        {
            temp = direction.xDirection();
            rect.setDimensions(0,GFX::getYResolution());
            // position left or right
            rect.setPosition((int)GFX::getXResolution() / 2 + (int)GFX::getXResolution() / 2 * temp,0);
        }
        else
        {
            temp = direction.yDirection();
            rect.setDimensions(GFX::getXResolution(),0);
            // position top or bottom
            rect.setPosition(0,(int)GFX::getYResolution() / 2 + (int)GFX::getYResolution() / 2 * temp);
        }
    }

    rect.setColour(wipeColour);
    timer = time;
    startTime = time;

    type = etWipe;
    limit = 1;
}

EffectWipe::~EffectWipe()
{

}

void EffectWipe::update()
{
    if (timer > 0)
    {
        if (isInverted)
        {
            if (direction.xDirection() != 0) // wipe left/right
            {
                int width = GFX::getXResolution() * ((float)timer / (float)startTime);
                rect.setWidth(width);
                if (direction.xDirection() < 0) // wipe from left to right (but inverted, so the box has to move across the screen)
                {
                    rect.setPosition((int)GFX::getXResolution() - width,0);
                }
            }
            else // wipe top/bottom
            {
                int height = GFX::getYResolution() * ((float)timer / (float)startTime);
                rect.setHeight(height);
                if (direction.yDirection() < 0) // wipe from top to bottom (but inverted, so the box has to move across the screen)
                {
                    rect.setPosition(0,(int)GFX::getYResolution() - height);
                }
            }
        }
        else
        {
            if (direction.xDirection() != 0) // wipe left/right
            {
                int width = GFX::getXResolution() * (1.0f - (float)timer / (float)startTime);
                rect.setWidth(width);
                if (direction.xDirection() > 0) // wipe from right to left
                {
                    // rectangle has to be positioned relative to right border of the screen
                    rect.setPosition((int)GFX::getXResolution() - width,0);
                }
            }
            else // wipe top/bottom
            {
                int height = GFX::getYResolution() * (1.0f - (float)timer / (float)startTime);
                rect.setHeight(height);
                if (direction.yDirection() > 0) // wipe from bottom to top
                {
                    rect.setPosition(0,(int)GFX::getYResolution() - height);
                }
            }
        }
    	--timer;
    }
    else
        finished = true;
}

void EffectWipe::render()
{
    rect.render();
}
