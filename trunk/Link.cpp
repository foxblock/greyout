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
	state = lsFadeOut;
	counter = LINK_FADE_OUT_TICKS;
}

void Link::render(SDL_Surface* screen)
{
	line.render(screen);
}

///--- PROTECTED ---------------------------------------------------------------

///--- PRIVATE -----------------------------------------------------------------
