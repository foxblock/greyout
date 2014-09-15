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

#ifndef EFFECTZOOM_H
#define EFFECTZOOM_H

#include "BaseEffect.h"

#include "Vector2df.h"

#include "Colour.h"
#include "Rectangle.h"
#include <SDL/SDL.h>

class EffectZoom : public BaseEffect
{
public:
	EffectZoom(CRint time, const Vector2df& pos, const Vector2df& newSize=Vector2df(1,1), const Colour& col=BLACK, CRbool inverted = false);
	virtual ~EffectZoom();

	// general
	virtual void update();

	virtual void render();

private:
	SDL_Surface* surf;
	Rectangle rect;
	Colour zoomCol;
	Vector2df position;
	Vector2df size;
	int timer;
	int zoomTime;
};

#endif // EFFECTZOOM_H


