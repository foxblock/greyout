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

#include "Link.h"

#include "BaseUnit.h"
#include "Level.h"

#define LINK_FADE_IN_TICKS 60 // 1 second
#define LINK_FADE_OUT_TICKS 60

Link::Link(Level *newParent, BaseUnit *src, BaseUnit *tgt)
{
	parent = newParent;
	source = src;
	target = tgt;
	state = lsFadeIn;
	counter = LINK_FADE_IN_TICKS;
	toBeRemoved = false;
	col.setColour(50,217,54,0);
}

Link::~Link()
{
	//
}


///--- PUBLIC ------------------------------------------------------------------

void Link::update()
{
	if (!source || !target)
		return;
	if (counter >= 0)
	{
		--counter;
		switch (state)
		{
		case lsFadeIn:
			col.setColour(50,217,54,255.0f * float(LINK_FADE_IN_TICKS - counter)/float(LINK_FADE_IN_TICKS));
			break;
		case lsFadeOut:
			col.setColour(50,217,54,255.0f * float(counter)/float(LINK_FADE_IN_TICKS));
			break;
		}
	}
	if (counter == 0)
	{
		switch (state)
		{
		case lsFadeIn:
			state = lsIdle;
			col.setColour(50,217,54,255);
			break;
		case lsFadeOut:
			toBeRemoved = true;
			col.setColour(50,217,54,0);
			break;
		}
	}
	line.setStartPosition(source->getPixel(diMIDDLE) - parent->drawOffset);
	line.setEndPosition(target->getPixel(diMIDDLE) - parent->drawOffset);
	line.setColour(col);
}

void Link::remove()
{
	if ( state != lsFadeOut )
	{
		state = lsFadeOut;
		counter = LINK_FADE_OUT_TICKS;
	}
}

void Link::render(SDL_Surface* screen)
{
	line.render(screen);
}

///--- PROTECTED ---------------------------------------------------------------

///--- PRIVATE -----------------------------------------------------------------
