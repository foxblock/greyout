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

#ifndef _LINK_H_
#define _LINK_H_

#include "PenjinTypes.h"
#include "Line.h"
#include "Colour.h"

class BaseUnit;
class Level;

class Link
{
public:
	Link(Level *newParent, BaseUnit *src, BaseUnit *tgt);
	virtual ~Link();

	void update();
	void remove();
	void render(SDL_Surface *screen);

	BaseUnit *source;
	BaseUnit *target;

	bool toBeRemoved;
protected:
	enum LinkState
	{
		lsNone=0,
		lsFadeIn,
		lsIdle,
		lsFadeOut
	};

	LinkState state;
	int counter;
	Line line;
	Colour col;
	Level *parent;
private:

};


#endif // _LINK_H_

